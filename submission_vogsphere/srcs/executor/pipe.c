/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:42 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 20:25:59 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
全体の処理の流れ

ft_execute_pipeline(shell)
│
├─① malloc で pipes・pids を確保
├─② open_all_pipes()   パイプを pipe_count 本作る
├─③ prepare_heredocs() heredoc を事前に処理する
├─④ fork_all_cmds()    コマンド数だけ fork する
│    │
│    └─ (子プロセス) exec_pipeline_child(i, count, pipes, shell)
│         │
│         ├─ set_child_io()       stdin/stdout をパイプに繋ぎ替える
│         ├─ close_all_pipes()    使い終わった fd を全部閉じる
│         ├─ shell->cmds を辿って i 番目の t_cmd を取得
│         ├─ ft_apply_redirs()    < > << >> のリダイレクトを適用
│         └─ exec_child_execve()  path検索 → execve → exit()
│
├─⑤ close_all_pipes()  親プロセスが全パイプを閉じる
├─⑥ wait_all_cmds()    全子プロセスの終了を待つ
└─⑦ free(pipes), free(pids)
*/

/*
** 【パイプ（pipe）の構造】
**
** pipe(pipefd) を呼ぶと、繋がった2つの口が作られる。
** データは必ず [1] から入って [0] から出る。一方通行。
**
**   pipefd[1]  ──→──→──→──  pipefd[0]
**   書込端(1)                  読込端(0)
**  「ここに書く」             「ここから読む」
**
** 覚え方: 0=stdin(読む), 1=stdout(書く) と同じ番号の感覚。
**
** -------------------------------------------------------
**
** 【ls | grep foo | wc -l の場合】
**
** パイプは「コマンド数 - 1」本作る。
** 3コマンド → pipes[0], pipes[1] の2本。
**
**   ls          pipes[0]        grep         pipes[1]        wc
**  ┌────┐      ┌──────────┐    ┌────┐      ┌──────────┐    ┌────┐
**  │    │ [1]→ │→→→→→→→→│→[0]│    │ [1]→ │→→→→→→→→│→[0]│    │
**  │stdout    └──────────┘   stdin│    │stdout    └──────────┘   stdin│    │
**  └────┘                    └────┘                             └────┘
**
** cmd_idx=0(ls)  : stdin=そのまま,  stdout=pipes[0][1] に繋ぐ
** cmd_idx=1(grep): stdin=pipes[0][0]に繋ぐ, stdout=pipes[1][1] に繋ぐ
** cmd_idx=2(wc)  : stdin=pipes[1][0]に繋ぐ, stdout=そのまま
**
** -------------------------------------------------------
**
** 【pipes 配列と cmd_idx の対応】
**
**   ls   |   grep   |   wc
**   [0]      [1]       [2]    ← cmd_idx
**        ↑        ↑
**     pipes[0]  pipes[1]      ← パイプの番号
**
** cmd_idx=1(grep) の左のパイプ = pipes[1-1] = pipes[cmd_idx-1]
** その読込端 = pipes[cmd_idx-1][0] を stdin に繋ぐ。
**
** -------------------------------------------------------
**
** 【親プロセスが全パイプを close() しなければならない理由】
**
** パイプの読込端([0])にEOFが届く条件：
**   「書込端([1])を持つプロセスが全員 close() すること」
**
** 親プロセスも fork() 後に pipes の fd を持ち続けている。
** 親が close() しないと書込端を持つプロセスが残り続けるため、
** 読込端の子プロセスはEOFを受け取れず永遠に待ち続ける（ハング）。
*/

#include "minishell.h"
#include <errno.h>
#include <sys/wait.h>

/*
** 子プロセスの stdin と stdout を付け替える。
** 最初のコマンドは stdin をそのまま、最後は stdout をそのまま残す。
** それ以外はパイプの対応する端に繋ぎ直す。
*/
static void	set_child_io(int idx, int count, int (*pipes)[2])
{
	if (idx > 0)
	{
		if (dup2(pipes[idx - 1][0], STDIN_FILENO) == -1)
		{
			perror("minishell: dup2 stdin");
			exit(1);
		}
	}
	if (idx < count - 1)
	{
		if (dup2(pipes[idx][1], STDOUT_FILENO) == -1)
		{
			perror("minishell: dup2 stdout");
			exit(1);
		}
	}
}

/*
** execve を呼び出す直前の処理をまとめたヘルパー。
** path の検索・envp の生成・execve の実行を行い、必ず exit() する。
*/
static void	exec_child_execve(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	**current_envp;
	int		status;

	path = search_path(cmd->args[0], shell->env);
	if (!path)
	{
		print_error_msg(cmd->args[0], NULL, "command not found");
		exit(127);
	}
	current_envp = env_to_envp(shell->env);
	if (!current_envp)
		exit((free(path), 1));
	execve(path, cmd->args, current_envp);
	status = 1;
	if (errno == ENOENT)
		status = 127;
	else if (errno == EACCES || errno == EISDIR)
		status = 126;
	perror("minishell: execve");
	free_envp(current_envp);
	free(path);
	exit(status);
}

/*
** 子プロセスの処理。
** stdin/stdout を付け替え→リダイレクト適用→execve の順で進む。
** shell->cmds を辿って idx 番目の t_cmd を取得する。
*/
static void	exec_pipeline_child(int idx, int count, int (*pipes)[2],
		t_shell *shell)
{
	t_cmd	*cmd;
	int		i;

	set_child_io(idx, count, pipes);
	close_all_pipes(pipes, count - 1);
	cmd = shell->cmds;
	i = 0;
	while (cmd && i < idx)
	{
		cmd = cmd->next;
		i++;
	}
	if (!cmd)
		exit(1);
	if (ft_apply_redirs(cmd) == -1)
		exit(1);
	exec_child_execve(cmd, shell);
}

/*
** 全コマンドを fork して子プロセスとして起動する。
** 各 pid を pids 配列に保存する。
** 戻り値: 成功 0 / 失敗 -1
*/
static int	fork_all_cmds(t_shell *shell, int cmd_count, int (*pipes)[2],
		pid_t *pids)
{
	int	i;

	i = 0;
	while (i < cmd_count)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("minishell: fork");
			return (-1);
		}
		if (pids[i] == 0)
			exec_pipeline_child(i, cmd_count, pipes, shell);
		i++;
	}
	return (0);
}

/*
** パイプラインを実行するメイン関数。
** ① pipes・pids を確保してパイプを全部作る
** ② heredoc を事前処理する
** ③ 全コマンドを fork して起動
** ④ 親は全パイプを close してから全子プロセスの終了を待つ
*/
void	ft_execute_pipeline(t_shell *shell)
{
	int		cmd_count;
	int		pipe_count;
	pid_t	*pids;
	int		(*pipes)[2];

	cmd_count = count_cmds(shell->cmds);
	pipe_count = cmd_count - 1;
	pipes = malloc(sizeof(*pipes) * pipe_count);
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pipes || !pids)
		return (free(pipes), free(pids), (void)0);
	if (open_all_pipes(pipes, pipe_count) == -1
		|| prepare_heredocs(shell->cmds) == -1 || fork_all_cmds(shell,
			cmd_count, pipes, pids) == -1)
	{
		close_all_pipes(pipes, pipe_count);
		free(pipes);
		free(pids);
		return ;
	}
	close_all_pipes(pipes, pipe_count);
	wait_all_cmds(pids, cmd_count, shell);
	free(pipes);
	free(pids);
}

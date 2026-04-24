/*
** 処理の流れ：
**
** コマンド数を数える
** (コマンド数 - 1) 個のパイプを作る
** コマンドの数だけ fork する
** 各子プロセスで stdin/stdout をパイプに繋ぎ替える
** 親プロセスは全 fd を close してから全子プロセスを wait する
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
#include "libft.h"
#include <sys/wait.h>

/*
** 子プロセスの stdin を付け替える。
** 最初のコマンド(idx=0)は変更なし。
** 2番目以降は1つ前のパイプの読込端を stdin に繋ぐ。
*/
static void	set_child_stdin(int cmd_idx, int (*pipes)[2])
{
	if (cmd_idx == 0)
		return ;
	// pipes[cmd_idx-1][0] = 1つ前のパイプの読込端
	if (dup2(pipes[cmd_idx - 1][0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2 stdin");
		exit(1);
	}
}

/*
** 子プロセスの stdout を付け替える。
** 最後のコマンドは変更なし。
** それ以外は自分のパイプの書込端を stdout に繋ぐ。
*/
static void	set_child_stdout(int cmd_idx, int cmd_count, int (*pipes)[2])
{
	if (cmd_idx == cmd_count - 1)
		return ;
	// pipes[cmd_idx][1] = このコマンドのパイプの書込端
	if (dup2(pipes[cmd_idx][1], STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2 stdout");
		exit(1);
	}
}


/*
** 子プロセスの処理。
** stdin/stdout を付け替えてからコマンドを execve で起動する。
** この関数の中で必ず exit() するので return はしない。
*/
static void	exec_pipeline_child(int cmd_idx, int cmd_count,
				int (*pipes)[2], t_cmd *cmd, t_shell *shell)
{
	char	*path;
	int		pipe_count;
	char	**current_envp;

	pipe_count = cmd_count - 1;
	set_child_stdin(cmd_idx, pipes);
	set_child_stdout(cmd_idx, cmd_count, pipes);
	close_all_pipes(pipes, pipe_count); // dup2済みなので全fd不要
	if (ft_apply_redirs(cmd) == -1)
		exit(1);
	path = search_path(cmd->args[0], shell->env);
	if (!path)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": command not found", STDERR_FILENO);
		exit(127);
	}
	current_envp = env_to_envp(shell->env);
	execve(path, cmd->args, current_envp);
	perror("minishell: execve"); // execve失敗時（通常ここには来ない）
	free(path);
	exit(1);
}

/*
** 全コマンドを fork して子プロセスとして起動する。
** 各 pid を pids 配列に保存する。
** 戻り値: 成功 0 / 失敗 -1
*/
static int	fork_all_cmds(t_shell *shell, int cmd_count,
				int (*pipes)[2], pid_t *pids)
{
	t_cmd	*cmd;
	int		i;

	cmd = shell->cmds;
	i = 0;
	while (cmd)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("minishell: fork");
			return (-1);
		}
		if (pids[i] == 0) // 子プロセスの処理
			exec_pipeline_child(i, cmd_count, pipes, cmd, shell);
		cmd = cmd->next;
		i++;
	}
	return (0);
}



/*
** パイプラインを実行するメイン関数。
** ① パイプを全部作る
** ② 全コマンドを fork して起動
** ③ 親は全パイプを close してから全子プロセスの終了を待つ
*/
void	ft_execute_pipeline(t_shell *shell)
{
	int		cmd_count;
	int		pipe_count;
	int		(*pipes)[2];
	pid_t	*pids;

	cmd_count = count_cmds(shell->cmds);
	pipe_count = cmd_count - 1;
	pipes = malloc(sizeof(int[2]) * pipe_count);
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pipes || !pids)
	{
		free(pipes);
		free(pids);
		return ;
	}
	if (open_all_pipes(pipes, pipe_count) == -1) // ① 全パイプを作る
	{
		free(pipes);
		free(pids);
		return ;
	}
	if (fork_all_cmds(shell, cmd_count, pipes, pids) == -1) // ② fork
	{
		close_all_pipes(pipes, pipe_count);
		free(pipes);
		free(pids);
		return ;
	}
	close_all_pipes(pipes, pipe_count); // ③ 親が全パイプを close
	wait_all_cmds(pids, cmd_count, shell); // ④ 全子の終了を待つ
	free(pipes);
	free(pids);
}

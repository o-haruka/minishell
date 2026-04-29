/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:31:17 by homura            #+#    #+#             */
/*   Updated: 2026/04/29 19:16:31 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <errno.h>
#include <sys/wait.h>

/*--------------------------------------------
** 1. ビルトインコマンドを実行する
** リダイレクト適用前後の stdio を dup で退避・復元する。
** リダイレクト失敗時もステータス設定と復元は必ず行う。
--------------------------------------------*/
static void	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	int	saved_stdout;
	int	saved_stdin;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	if (ft_apply_redirs(cmd) != -1)
		shell->last_status = exec_builtin(cmd, shell);
	else
		shell->last_status = 1;
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}

/*--------------------------------------------
** 2. 外部コマンドを fork して実行する
** 子: シグナルをデフォルトに戻し、リダイレクトを適用してから execve
** 親: wait_for_child で子の終了を待ち、ステータスを反映する
--------------------------------------------*/
static void	exec_external(char *path, t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		print_error_msg(NULL, "fork", strerror(errno));
		free(path);
		return ;
	}
	if (pid == 0)
	{
		set_signal_for_child();
		if (ft_apply_redirs(cmd) == -1)
		{
			free(path);
			exit(1);
		}
		do_execve(path, cmd, shell);
	}
	if (wait_for_child(pid, &status) != -1)
		update_last_status(status, shell);
	free(path);
}

/*--------------------------------------------
** 3. コマンドを振り分けて実行するエントリポイント
** パイプあり  → ft_execute_pipeline に委譲
** ビルトイン → execute_builtin で直接実行
** 外部コマンド → パスを検索して exec_external でフォーク実行
--------------------------------------------*/
void	ft_execute(t_shell *shell)
{
	t_cmd	*cmd;
	char	*path;

	cmd = shell->cmds;
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (cmd->next != NULL)
		ft_execute_pipeline(shell);
	else if (is_builtin(cmd->args[0]))
		execute_builtin(cmd, shell);
	else
	{
		path = search_path(cmd->args[0], shell->env);
		if (!path)
		{
			print_error_msg(NULL, cmd->args[0], "command not found");
			shell->last_status = 127;
			return ;
		}
		exec_external(path, cmd, shell);
	}
}

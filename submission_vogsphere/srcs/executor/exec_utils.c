/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 00:00:00 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/29 00:00:00 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** execve 失敗後の errno を POSIX 準拠の終了ステータスに変換する。
** ENOENT=127（コマンド未発見）、EACCES/EISDIR=126（実行権限なし）。
*/
int	execve_exit_status(void)
{
	if (errno == ENOENT)
		return (127);
	if (errno == EACCES || errno == EISDIR)
		return (126);
	return (1);
}

/*
** path・envp・execve をまとめて処理し、必ず exit() する。
** execve 失敗時はコマンド名付きエラーを出力してから終了する。
*/
void	do_execve(char *path, t_cmd *cmd, t_shell *shell)
{
	char	**current_envp;
	int		status;

	current_envp = env_to_envp(shell->env);
	if (!current_envp)
	{
		free(path);
		exit(1);
	}
	execve(path, cmd->args, current_envp);
	status = execve_exit_status();
	print_error_msg(NULL, cmd->args[0], strerror(errno));
	free_envp(current_envp);
	free(path);
	exit(status);
}

/*
** シグナルを待機用に設定してから waitpid し、終わったら元に戻す。
** 成功: 0 / 失敗（waitpid エラー）: -1
*/
int	wait_for_child(pid_t pid, int *status)
{
	set_signal_for_parent_wait();
	if (waitpid(pid, status, 0) == -1)
	{
		print_error_msg(NULL, "waitpid", strerror(errno));
		setup_signals();
		return (-1);
	}
	setup_signals();
	return (0);
}

/*
** waitpid で取得した status を shell->last_status に反映する。
** シグナル終了時は bash に合わせた見た目処理も行う。
*/
void	update_last_status(int status, t_shell *shell)
{
	if (WIFEXITED(status))
		shell->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		shell->last_status = 128 + WTERMSIG(status);
		if (WTERMSIG(status) == SIGQUIT)
			ft_putendl_fd("Quit: 3", STDERR_FILENO);
		else if (WTERMSIG(status) == SIGINT)
			write(STDERR_FILENO, "\n", 1);
	}
}

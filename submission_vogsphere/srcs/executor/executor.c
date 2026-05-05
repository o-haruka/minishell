/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:31:17 by homura            #+#    #+#             */
/*   Updated: 2026/05/05 10:08:27 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <errno.h>
#include <sys/wait.h>

/*
** Save stdin and stdout by dup-ing them before a builtin runs.
** Returns 0 on success, -1 on failure. Called by execute_builtin.
*/
static int	save_stdio(int *saved_stdout, int *saved_stdin)
{
	*saved_stdout = dup(STDOUT_FILENO);
	if (*saved_stdout == -1)
	{
		print_error_msg(NULL, "dup stdout", strerror(errno));
		return (-1);
	}
	*saved_stdin = dup(STDIN_FILENO);
	if (*saved_stdin == -1)
	{
		print_error_msg(NULL, "dup stdin", strerror(errno));
		close(*saved_stdout);
		*saved_stdout = -1;
		return (-1);
	}
	return (0);
}

/*
** Restore stdin and stdout from the saved fds, then close them.
** Returns 0 on success, -1 on failure. Called by execute_builtin.
*/
static int	restore_stdio(int saved_stdout, int saved_stdin)
{
	if (saved_stdout != -1)
	{
		if (dup2(saved_stdout, STDOUT_FILENO) == -1)
		{
			print_error_msg(NULL, "dup2 stdout", strerror(errno));
			close(saved_stdout);
			if (saved_stdin != -1)
				close(saved_stdin);
			return (-1);
		}
		close(saved_stdout);
	}
	if (saved_stdin != -1)
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1)
		{
			print_error_msg(NULL, "dup2 stdin", strerror(errno));
			close(saved_stdin);
			return (-1);
		}
		close(saved_stdin);
	}
	return (0);
}

/*
** Run a builtin command in the current process.
** Saves/restores stdin and stdout around redirections so they don't persist.
** Called by ft_execute when the command is a builtin.
*/
static void	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	int	saved_stdout;
	int	saved_stdin;

	saved_stdout = -1;
	saved_stdin = -1;
	if (save_stdio(&saved_stdout, &saved_stdin) == -1)
	{
		shell->last_status = 1;
		return ;
	}
	if (ft_apply_redirs(cmd) == 0)
		shell->last_status = call_builtin(cmd, shell);
	else
	{
		if (g_signal != 0)
			shell->last_status = 128 + g_signal;
		else
			shell->last_status = 1;
	}
	if (restore_stdio(saved_stdout, saved_stdin) == -1)
		shell->last_status = 1;
}

/*
** Fork and run an external command at the resolved path.
** Child resets signals, applies redirections, then calls execve.
** Parent waits for the child and updates last_status.
*/
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
		exec_child(path, cmd, shell);
	if (wait_for_child(pid, &status) == 0)
		update_last_status(status, shell);
	else
		shell->last_status = 1;
	free(path);
}

/*
** Entry point for command execution, called from the main shell loop.
** Dispatches to ft_execute_pipeline (pipes), execute_builtin, or exec_external
** depending on whether the command list has pipes or is a builtin.
*/
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

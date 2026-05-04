/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 00:00:00 by hkuninag          #+#    #+#             */
/*   Updated: 2026/05/04 13:02:46 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Maps errno after a failed execve to a POSIX exit status.
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
** Converts env list to envp and calls execve. On failure, prints an error
** and exits. Called in exec_external and exec_pipeline_child; never returns.
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

	if (is_directory(path))
	{
		print_error_msg(NULL, cmd->args[0], "Is a directory");
		free_envp(current_envp);
		free(path);
		exit(126);
	}

	execve(path, cmd->args, current_envp);
	status = execve_exit_status();
	print_error_msg(NULL, cmd->args[0], strerror(errno));
	free_envp(current_envp);
	free(path);
	exit(status);
}

/*
** Sets signals to wait mode, calls waitpid, then restores signals.
** Returns 0 on success, -1 on waitpid error. Called by exec_external
** and read_heredoc_input.
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
** Translates a raw waitpid status into shell->last_status.
** Handles normal exit and signal termination (128+signum).
** Called after every waitpid.
*/
void	update_last_status(int status, t_shell *shell)
{
	int	sig;

	if (WIFEXITED(status))
		shell->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		shell->last_status = 128 + sig;
		if (sig == SIGQUIT)
		{
			if (WCOREDUMP(status))
				ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
			else
				ft_putendl_fd("Quit", STDERR_FILENO);
		}
		else if (sig == SIGINT)
			write(STDERR_FILENO, "\n", 1);
	}
}

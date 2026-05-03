/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:50 by hkuninag          #+#    #+#             */
/*   Updated: 2026/05/01 15:41:23 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/wait.h>

/*
** Child process for heredoc: reads lines until the delimiter or EOF,
** writing each line into the pipe. Never returns.
*/
static void	exec_heredoc_child(int *pipefd, char *delimiter)
{
	char	*line;

	set_signal_for_heredoc_child();
	close(pipefd[0]);
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
			&& ft_strlen(line) == ft_strlen(delimiter))
		{
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	exit(0);
}

/*
** Forks a child to collect heredoc input into a pipe.
** Returns the read-end fd on success, -1 on failure or Ctrl+D interrupt.
** Called by prepare_heredocs and apply_heredoc_redir.
*/
static int	apply_heredoc_wait(pid_t pid, int *pipefd)
{
	int	status;

	if (wait_for_child(pid, &status) == -1)
	{
		close(pipefd[0]);
		return (-1);
	}
	if (WIFSIGNALED(status))
	{
		g_signal = WTERMSIG(status);
		if (g_signal == SIGINT)
			write(STDERR_FILENO, "\n", 1);
		close(pipefd[0]);
		return (-1);
	}
	return (pipefd[0]);
}

int	read_heredoc_input(char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (print_error_msg(NULL, "pipe", strerror(errno)), -1);
	pid = fork();
	if (pid == -1)
	{
		print_error_msg(NULL, "fork", strerror(errno));
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		exec_heredoc_child(pipefd, delimiter);
	close(pipefd[1]);
	return (apply_heredoc_wait(pid, pipefd));
}

/*
** Pre-processes all heredoc redirections before forking, storing
** the read-end fd in redir->fd. Called by ft_execute_pipeline.
*/
int	prepare_heredocs(t_cmd *cmd)
{
	t_redir	*redir;

	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->kind == TK_HEREDOC)
			{
				redir->fd = read_heredoc_input(redir->file);
				if (redir->fd == -1)
					return (-1);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0);
}

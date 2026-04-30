/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:33 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:44:37 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Returns the number of commands in the linked list.
** Used by ft_execute_pipeline to size the pipes and pids arrays.
*/
int	count_cmds(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

/*
** Creates pipe_count pipes. On failure, closes all already-opened fds
** and returns -1. Called by ft_execute_pipeline.
*/
int	open_all_pipes(int (*pipes)[2], int pipe_count)
{
	int	i;
	int	j;

	i = 0;
	while (i < pipe_count)
	{
		if (pipe(pipes[i]) == -1)
		{
			print_error_msg(NULL, "pipe", strerror(errno));
			j = 0;
			while (j < i)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
				j++;
			}
			return (-1);
		}
		i++;
	}
	return (0);
}

/*
** Closes both ends of every pipe. Called in children after dup2
** and in the parent after forking.
*/
void	close_all_pipes(int (*pipes)[2], int pipe_count)
{
	int	i;

	i = 0;
	while (i < pipe_count)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

/*
** Returns the nth node in the command linked list.
** Used by exec_pipeline_child to find the command for a given child index.
*/
t_cmd	*get_nth_cmd(t_cmd *cmd, int idx)
{
	while (cmd && idx > 0)
	{
		cmd = cmd->next;
		idx--;
	}
	return (cmd);
}

/*
** Waits for all pipeline children and updates shell->last_status
** from the last command's exit status. Called by ft_execute_pipeline.
*/
void	wait_all_cmds(pid_t *pids, int cmd_count, t_shell *shell)
{
	int	i;
	int	status;

	i = 0;
	status = 0;
	set_signal_for_parent_wait();
	while (i < cmd_count)
	{
		if (waitpid(pids[i], &status, 0) == -1)
			print_error_msg(NULL, "waitpid", strerror(errno));
		i++;
	}
	update_last_status(status, shell);
	setup_signals();
}

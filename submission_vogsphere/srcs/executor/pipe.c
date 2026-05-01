/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:42 by hkuninag          #+#    #+#             */
/*   Updated: 2026/05/01 15:38:47 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Wire stdin/stdout of this child to the correct pipe ends.
** Not the first cmd (idx > 0): read from the previous pipe's read end.
** Not the last cmd (idx < count-1): write to the current pipe's write end.
** Called by exec_pipeline_child before closing all pipe fds.
*/
static void	set_child_io(int idx, int count, int (*pipes)[2])
{
	if (idx > 0)
	{
		if (dup2(pipes[idx - 1][0], STDIN_FILENO) == -1)
		{
			print_error_msg(NULL, "dup2 stdin", strerror(errno));
			exit(1);
		}
	}
	if (idx < count - 1)
	{
		if (dup2(pipes[idx][1], STDOUT_FILENO) == -1)
		{
			print_error_msg(NULL, "dup2 stdout", strerror(errno));
			exit(1);
		}
	}
}

/*
** Run inside the child process for one pipeline stage.
** Sets up I/O, closes all inherited pipe fds, applies redirections,
** then execves. Never returns.
*/
static void	exec_pipeline_child(int idx, int count, int (*pipes)[2],
		t_shell *shell)
{
	t_cmd	*cmd;
	char	*path;

	set_signal_for_child();
	set_child_io(idx, count, pipes);
	close_all_pipes(pipes, count - 1);
	cmd = get_nth_cmd(shell->cmds, idx);
	if (!cmd)
		exit(1);
	if (ft_apply_redirs(cmd) == -1)
	{
		if (g_signal != 0)
			exit(128 + g_signal);
		exit(1);
	}
	path = search_path(cmd->args[0], shell->env);
	if (!path)
	{
		print_error_msg(NULL, cmd->args[0], "command not found");
		exit(127);
	}
	do_execve(path, cmd, shell);
}

/*
** Fork one child per command. All children are created before any are
** waited on so they run concurrently. Called by ft_execute_pipeline.
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
			print_error_msg(NULL, "fork", strerror(errno));
			return (-1);
		}
		if (pids[i] == 0)
			exec_pipeline_child(i, cmd_count, pipes, shell);
		i++;
	}
	return (0);
}

static void	pipeline_failure(t_shell *shell, int (*pipes)[2], int pipe_count,
		pid_t *pids)
{
	if (g_signal != 0)
		shell->last_status = 128 + g_signal;
	else
		shell->last_status = 1;
	close_all_pipes(pipes, pipe_count);
	free(pipes);
	free(pids);
}

/*
** Entry point for pipeline execution, called by ft_execute when cmd->next
** is non-NULL. Allocates pipes and pids, forks all children, waits for
** them, then cleans up.
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
		return (free(pipes), free(pids));
	if (open_all_pipes(pipes, pipe_count) == -1
		|| prepare_heredocs(shell->cmds) == -1 || fork_all_cmds(shell,
			cmd_count, pipes, pids) == -1)
	{
		pipeline_failure(shell, pipes, pipe_count, pids);
		return ;
	}
	close_all_pipes(pipes, pipe_count);
	wait_all_cmds(pids, cmd_count, shell);
	free(pipes);
	free(pids);
}

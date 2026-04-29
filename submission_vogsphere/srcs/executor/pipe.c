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
** Pipeline execution flow:
**
**   ft_execute_pipeline
**    ├─ ① malloc: pipes, pids
**    ├─ ② open_all_pipes()
**    ├─ ③ prepare_heredocs()
**    ├─ ④ fork_all_cmds()
**    │    └─ exec_pipeline_child(i)
**    │         ├─ set_child_io()      — wire stdin/stdout to pipes
**    │         ├─ close_all_pipes()   — close inherited fds
**    │         ├─ ft_apply_redirs()   — apply redirections
**    │         └─ exec_child_execve() — execve or exit(127)
**    ├─ ⑤ close_all_pipes()  — parent closes its copies
**    ├─ ⑥ wait_all_cmds()
**    └─ ⑦ free: pipes, pids
**
** Pipe index layout (ls | grep foo | wc -l):
**
**   cmd idx:  [0] ls     [1] grep     [2] wc
**   pipe:          pipes[0]   pipes[1]
**
**   idx 0: stdout → pipes[0][1]
**   idx 1: stdin  ← pipes[0][0],  stdout → pipes[1][1]
**   idx 2: stdin  ← pipes[1][0]
**
** The parent must close all pipes after fork so that children
** reading from a pipe receive EOF when the upstream writer exits.
*/

#include "minishell.h"

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
		exit(1);
	path = search_path(cmd->args[0], shell->env);
	if (!path)
	{
		print_error_msg(NULL, cmd->args[0], "command not found");
		exit(127);
	}
	do_execve(path, cmd, shell);
}

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
		|| prepare_heredocs(shell->cmds) == -1
		|| fork_all_cmds(shell, cmd_count, pipes, pids) == -1)
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

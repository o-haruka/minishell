/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:59 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/29 00:00:00 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <fcntl.h>

static int	apply_heredoc_redir(t_redir *redir)
{
	if (redir->fd == -1)
		redir->fd = apply_heredoc(redir->file);
	if (redir->fd == -1 || dup2(redir->fd, STDIN_FILENO) == -1)
		return (-1);
	close(redir->fd);
	return (0);
}

static int	open_file_redir(t_redir *redir, int *target_fd)
{
	if (redir->kind == TK_REDIRECT_IN)
	{
		*target_fd = STDIN_FILENO;
		return (open(redir->file, O_RDONLY));
	}
	*target_fd = STDOUT_FILENO;
	if (redir->kind == TK_REDIRECT_OUT)
		return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
}

static int	apply_file_redir(t_redir *redir)
{
	int	fd;
	int	target_fd;

	fd = open_file_redir(redir, &target_fd);
	if (fd == -1)
	{
		print_error_msg(NULL, redir->file, strerror(errno));
		return (-1);
	}
	if (dup2(fd, target_fd) == -1)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

int	ft_apply_redirs(t_cmd *cmd)
{
	t_redir	*r;
	int		ret;

	if (!cmd)
		return (0);
	r = cmd->redirs;
	while (r)
	{
		if (r->kind == TK_HEREDOC)
			ret = apply_heredoc_redir(r);
		else
			ret = apply_file_redir(r);
		if (ret == -1)
			return (-1);
		r = r->next;
	}
	return (0);
}

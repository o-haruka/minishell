/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:50 by homura            #+#    #+#             */
/*   Updated: 2026/05/01 02:37:26 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Builtin pwd: print the current working directory to stdout.
** Passes NULL/0 to getcwd so it allocates the buffer automatically.
*/
int	ft_pwd(void)
{
	char	*path;

	path = getcwd(NULL, 0);
	if (path != NULL)
	{
		ft_putendl_fd(path, STDOUT_FILENO);
		free(path);
		return (0);
	}
	else
	{
		print_error_msg("pwd", NULL, strerror(errno));
		return (1);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:36:19 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:46:03 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <errno.h>
#include <string.h>

/*
** Write a formatted error to stderr: "minishell: [cmd: ][target: ]message".
** NULL parts are omitted. Used throughout builtins and executor.
*/
void	print_error_msg(char *cmd_name, char *target, char *error_msg)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (cmd_name != NULL)
	{
		ft_putstr_fd(cmd_name, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (target != NULL)
	{
		ft_putstr_fd(target, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putendl_fd(error_msg, STDERR_FILENO);
}

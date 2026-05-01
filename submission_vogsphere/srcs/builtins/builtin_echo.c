/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:36 by homura            #+#    #+#             */
/*   Updated: 2026/05/01 01:52:48 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Return true if arg is a valid -n flag ("-n", "-nn", "-nnn", ...).
** Called by ft_echo to detect the no-newline option.
*/
static bool	is_n_option(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (false);
	i = 2;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

/*
** Builtin echo: print args separated by spaces.
** Suppresses the trailing newline if the -n flag is set.
*/
int	ft_echo(t_cmd *cmd)
{
	int		i;
	bool	n_flag;

	if (!cmd || !cmd->args)
		return (0);
	n_flag = false;
	i = 1;
	while (cmd->args[i] && is_n_option(cmd->args[i]))
	{
		n_flag = true;
		i++;
	}
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], STDOUT_FILENO);
		if (cmd->args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (!n_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

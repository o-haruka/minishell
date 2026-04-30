/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:13 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 20:41:39 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Free all shell resources then call exit, clamping code to 0-255.
*/
static void	ft_do_exit(t_shell *shell, int code)
{
	free_env(shell->env);
	free_cmds_list(shell->cmds);
	token_free(&shell->tokens);
	exit(code & 0xFF);
}

/*
** Builtin exit: exit with last_status if no arg, or with the numeric arg
** if given. Prints an error and does not exit on non-numeric or too many args.
*/
int	ft_exit(t_cmd *cmd, t_shell *shell)
{
	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!cmd->args[1])
		ft_do_exit(shell, shell->last_status);
	if (!ft_islonglong(cmd->args[1]))
	{
		print_error_msg("exit", cmd->args[1], "numeric argument required");
		shell->last_status = 2;
		return (2);
	}
	if (cmd->args[2])
	{
		print_error_msg("exit", NULL, "too many arguments");
		shell->last_status = 1;
		return (1);
	}
	ft_do_exit(shell, ft_atoll(cmd->args[1]));
	return (0);
}

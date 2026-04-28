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
** 動的確保したメモリを解放してから exit() を呼ぶ。
** code & 0xFF で終了コードを 0〜255 に収める。
** (例: 256 → 0、257 → 1、-1 → 255)
*/
static void	ft_do_exit(t_shell *shell, int code)
{
	free_env(shell->env);
	free_cmds_list(shell->cmds);
	token_free(&shell->tokens);
	exit(code & 0xFF);
}

/*
** exit ビルトインの実装。
** 引数なし        → last_status の値で終了
** 引数1つ(数値)   → その値で終了
** 引数1つ(非数値) → エラーを表示して終了しない
** 引数2つ以上     → エラーを表示して終了しない
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

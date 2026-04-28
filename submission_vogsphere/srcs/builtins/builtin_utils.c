/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:31:46 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:44:19 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// ビルトインコマンドかどうかを判定する (1: ビルトイン, 0: 外部コマンド)
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "env", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	return (0);
}

// ビルトインコマンドを実行し、終了ステータス（last_status）を返す
int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	char	*name;

	name = cmd->args[0];
	if (ft_strncmp(name, "pwd", 4) == 0)
		return (ft_pwd());
	if (ft_strncmp(name, "env", 4) == 0)
		return (ft_env(shell));
	if (ft_strncmp(name, "echo", 5) == 0)
		return (ft_echo(cmd));
	if (ft_strncmp(name, "cd", 3) == 0)
		return (ft_cd(cmd, shell));
	if (ft_strncmp(name, "export", 7) == 0)
		return (ft_export(cmd, shell));
	if (ft_strncmp(name, "unset", 6) == 0)
		return (ft_unset(cmd, shell));
	if (ft_strncmp(name, "exit", 5) == 0)
		return (ft_exit(cmd, shell));
	return (0);
}

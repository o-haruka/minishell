/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:29:53 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:44:08 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Print all env variables in "declare -x KEY=\"VALUE\"" format.
** Variables without a value are printed as "declare -x KEY".
** Called by ft_export when no arguments are given.
*/
static void	ft_print_export(t_env *env)
{
	while (env)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(env->key, STDOUT_FILENO);
		if (env->value != NULL)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(env->value, STDOUT_FILENO);
			ft_putchar_fd('"', STDOUT_FILENO);
		}
		ft_putchar_fd('\n', STDOUT_FILENO);
		env = env->next;
	}
}

/*
** Return 1 if key is a valid env variable name: must start with alpha or '_',
** followed by alphanumeric or '_' characters only. Returns 0 otherwise.
*/
static int	ft_is_valid_key(char *key)
{
	int	i;

	if (!key || key[0] == '\0')
		return (0);
	if (!ft_isalpha(key[0]) && key[0] != '_')
		return (0);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/*
** Parse "KEY=VALUE", validate the key, then update the env list.
** Called by ft_export_var when '=' is found in the argument.
*/
static int	ft_export_with_eq(t_shell *shell, char *arg, char *eq)
{
	char	*key;
	char	*value;

	key = ft_substr(arg, 0, eq - arg);
	value = eq + 1;
	if (!key)
		return (1);
	if (!ft_is_valid_key(key))
	{
		print_error_msg("export", arg, "not a valid identifier");
		free(key);
		return (1);
	}
	if (update_env_value(&shell->env, key, value) != 0)
	{
		ft_putendl_fd("minishell: export: malloc failed", STDERR_FILENO);
		free(key);
		return (1);
	}
	free(key);
	return (0);
}

/*
** Process one export argument: parse KEY or KEY=VALUE and update the env list.
** Delegates to ft_export_with_eq when '=' is found.
*/
static int	ft_export_var(t_shell *shell, char *arg)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq != NULL)
		return (ft_export_with_eq(shell, arg, eq));
	if (!ft_is_valid_key(arg))
	{
		print_error_msg("export", arg, "not a valid identifier");
		return (1);
	}
	return (update_env_value(&shell->env, arg, NULL));
}

/*
** Builtin export: print all env vars in declare-format if no args,
** otherwise parse and register each argument in the env list.
*/
int	ft_export(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	has_error;

	if (!cmd->args[1])
	{
		ft_print_export(shell->env);
		return (0);
	}
	has_error = 0;
	i = 1;
	while (cmd->args[i])
	{
		if (ft_export_var(shell, cmd->args[i]) != 0)
			has_error = 1;
		i++;
	}
	return (has_error);
}

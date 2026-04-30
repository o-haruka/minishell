/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:03 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 17:11:30 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>

/*
** Resolve the target path for cd. Returns $HOME when no arg is given,
** NULL on error. Called by ft_cd.
*/
static char	*get_cd_path(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->args[1] == NULL)
	{
		path = get_env_value(shell->env, "HOME");
		if (path == NULL)
			print_error_msg("cd", NULL, "HOME not set");
		return (path);
	}
	else if (cmd->args[2] != NULL)
	{
		print_error_msg("cd", NULL, "too many arguments");
		return (NULL);
	}
	return (cmd->args[1]);
}

/*
** Error cleanup for update_pwd_vars: prints an error and frees both paths.
*/
static int	update_pwd_error(char *old_pwd, char *new_pwd)
{
	print_error_msg(NULL, "malloc", strerror(errno));
	free(old_pwd);
	free(new_pwd);
	return (1);
}

/*
** Update PWD and OLDPWD in the env list after a successful chdir.
** Called by ft_cd.
*/
static int	update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (old_pwd)
	{
		if (update_env_value(&(shell->env), "OLDPWD", old_pwd) != 0)
			return (update_pwd_error(old_pwd, new_pwd));
	}
	if (new_pwd)
	{
		if (update_env_value(&(shell->env), "PWD", new_pwd) != 0)
			return (update_pwd_error(old_pwd, new_pwd));
	}
	free(old_pwd);
	free(new_pwd);
	return (0);
}

/*
** Builtin cd: resolve the target path, chdir to it, then update PWD/OLDPWD.
*/
int	ft_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	*old_pwd;

	path = get_cd_path(cmd, shell);
	if (path == NULL)
		return (1);
	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
	{
		free(old_pwd);
	}
	if (chdir(path) != 0)
	{
		print_error_msg("cd", path, strerror(errno));
		free(old_pwd);
		return (1);
	}
	return (update_pwd_vars(shell, old_pwd));
}

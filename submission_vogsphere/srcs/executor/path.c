/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:30 by homura            #+#    #+#             */
/*   Updated: 2026/05/05 10:08:50 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

/*
** Free a NULL-terminated array of strings and the array itself.
** Helper used only within path.c.
*/
static void	free_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/*
** Check if the given path is a directory.
** Returns 1 if true, 0 if false.
*/
int	is_directory(const char *path)
{
	struct stat	statbuf;

	if (stat(path, &statbuf) != 0)
		return (0);
	return (S_ISDIR(statbuf.st_mode));
}

/*
** Concatenates a directory and a command name with '/' between them.
** Helper for find_executable_in_paths.
*/
static char	*join_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full_path;

	tmp = ft_strjoin(dir, "/");
	if (tmp == NULL)
		return (NULL);
	full_path = ft_strjoin(tmp, cmd);
	free(tmp);
	return (full_path);
}

/*
** Iterates over a PATH directory array and returns the first executable
** path found via access(X_OK). Returns NULL if no match.
*/
static char	*find_executable_in_paths(char **paths, char *cmd)
{
	char	*full_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		full_path = join_path(paths[i], cmd);
		if (full_path == NULL)
			return (NULL);
		if (access(full_path, X_OK) == 0 && !is_directory(full_path))
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

/*
** Resolves a command name to an executable path. Handles absolute/relative
** paths directly; otherwise splits PATH and searches each directory.
** Called by exec_external and exec_pipeline_child.
*/
char	*search_path(char *cmd, t_env *env)
{
	char	**paths;
	char	*path_env;
	char	*exec_path;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (paths == NULL)
		return (NULL);
	exec_path = find_executable_in_paths(paths, cmd);
	free_array(paths);
	return (exec_path);
}

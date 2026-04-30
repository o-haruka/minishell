/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_envp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 00:00:00 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/29 00:00:00 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Free a char** envp array created by env_to_envp.
** Frees each "KEY=VALUE" string and then the array itself.
*/
void	free_envp(char **envp)
{
	int	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
		free(envp[i++]);
	free(envp);
}

/*
** Build a "KEY=VALUE" string from one t_env node.
** Nodes with a NULL value are treated as "KEY=". Called by fill_envp.
*/
static char	*make_kv_pair(t_env *node)
{
	char	*tmp_str;
	char	*kv;
	char	*val;

	tmp_str = ft_strjoin(node->key, "=");
	if (!tmp_str)
		return (NULL);
	val = node->value;
	if (!val)
		val = "";
	kv = ft_strjoin(tmp_str, val);
	free(tmp_str);
	return (kv);
}

/*
** Fill the envp array from the env list. On failure, frees already-filled
** entries; the array itself is freed by the caller. Called by env_to_envp.
*/
static int	fill_envp(t_env *env, char **envp)
{
	char	*kv;
	int		i;

	i = 0;
	while (env)
	{
		kv = make_kv_pair(env);
		if (!kv)
		{
			while (i > 0)
				free(envp[--i]);
			return (0);
		}
		envp[i] = kv;
		i++;
		env = env->next;
	}
	envp[i] = NULL;
	return (1);
}

/*
** Convert the t_env list to a NULL-terminated char** array for execve.
** Caller must free the result with free_envp.
*/
char	**env_to_envp(t_env *env)
{
	char	**envp;
	t_env	*tmp;
	int		count;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	envp = malloc(sizeof(char *) * (count + 1));
	if (!envp)
		return (NULL);
	if (!fill_envp(env, envp))
	{
		free(envp);
		return (NULL);
	}
	return (envp);
}

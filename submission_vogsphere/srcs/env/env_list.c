/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:02 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/29 00:00:00 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Allocate a new t_env node from a "KEY=VALUE" string.
** Duplicates key and value separately. Returns NULL on failure.
*/
static t_env	*ft_new_env(char *entry)
{
	t_env	*node;
	char	*eq;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		free(node);
		return (NULL);
	}
	node->key = ft_substr(entry, 0, eq - entry);
	node->value = ft_strdup(eq + 1);
	node->next = NULL;
	if (!node->key || !node->value)
	{
		free(node->key);
		free(node->value);
		free(node);
		return (NULL);
	}
	return (node);
}

/*
** Convert a char** envp array into a t_env linked list.
** Called once at startup from main to initialize shell->env.
*/
t_env	*init_env(char **envp)
{
	t_env	*head;
	t_env	*tail;
	t_env	*node;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (envp[i])
	{
		node = ft_new_env(envp[i]);
		if (!node)
		{
			free_env(head);
			return (NULL);
		}
		if (!head)
			head = node;
		else
			tail->next = node;
		tail = node;
		i++;
	}
	return (head);
}

/*
** Free the entire t_env list including all keys, values, and nodes.
*/
void	free_env(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = tmp;
	}
}

/*
** Find a key in the env list and return its value, or NULL if not found.
*/
char	*get_env_value(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strncmp(env->key, key, ft_strlen(key)) == 0
			&& ft_strlen(env->key) == ft_strlen(key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** Append new_node to the end of the env list.
** Called by add_new_env_node when registering a new variable.
*/
void	env_add_back(t_env **head, t_env *new_node)
{
	t_env	*current;

	if (!head || !new_node)
		return ;
	if (*head == NULL)
		*head = new_node;
	else
	{
		current = *head;
		while (current->next != NULL)
			current = current->next;
		current->next = new_node;
	}
}

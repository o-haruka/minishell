/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_update.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:31:04 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 17:09:15 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** key・value で新規ノードを作り、リスト末尾に追加する。
** update_env_value から key が未登録だった場合に呼ばれる。
*/
static int	add_new_env_node(t_env **env_head, char *key, char *value)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (1);
	new_node->key = ft_strdup(key);
	new_node->value = NULL;
	if (value)
		new_node->value = ft_strdup(value);
	new_node->next = NULL;
	if (!new_node->key || (value && !new_node->value))
	{
		free(new_node->key);
		free(new_node->value);
		free(new_node);
		return (1);
	}
	env_add_back(env_head, new_node);
	return (0);
}

/*
** key が既存なら value を上書き、未登録なら新規ノードを追加する。
*/
int	update_env_value(t_env **env_head, char *key, char *new_value)
{
	t_env	*current;

	if (!env_head || !key)
		return (1);
	current = *env_head;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
		{
			free(current->value);
			current->value = NULL;
			if (new_value)
			{
				current->value = ft_strdup(new_value);
				if (!current->value)
					return (1);
			}
			else
				current->value = NULL;
			return (0);
		}
		current = current->next;
	}
	return (add_new_env_node(env_head, key, new_value));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:02 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:44:21 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** "KEY=VALUE" 形式の文字列から t_env を1つ作って返す。
** key と value はそれぞれ ft_strdup でコピーして保持する。
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
** envp（文字列配列）を走査し、t_env の連結リストに変換して返す。
** 例) envp = {"HOME=/home/rex", "USER=rex", NULL}
**     → [key=HOME, value=/home/rex] → [key=USER, value=rex] → NULL
** main.c の初期化処理から呼ばれる。
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

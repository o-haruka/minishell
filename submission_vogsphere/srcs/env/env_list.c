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

/*
** t_env リスト全体を解放する。
** key・value・ノード本体の順で free する。
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
** t_env リストから key が一致するノードを探し、value を返す。
** 見つからなければ NULL を返す。
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
** t_env リストの末尾に new_node を追加する。
** ft_lstadd_back 相当のリスト構造操作。
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

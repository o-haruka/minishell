/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:34:49 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:45:36 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 新しいトークンを作成する
t_token	*token_new(char *word, t_token_kind kind)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->word = word;
	token->kind = kind;
	token->next = NULL;
	return (token);
}

// リストの末尾に追加する
void	token_add_back(t_token **head, t_token *new_token)
{
	t_token	*last;

	if (!head || !new_token)
		return ;
	if (*head == NULL)
	{
		*head = new_token;
		return ;
	}
	last = *head;
	while (last->next != NULL)
		last = last->next;
	last->next = new_token;
}

// トークンリスト全体を解放する
void	token_free(t_token **head)
{
	t_token	*current;
	t_token	*next;

	if (!head || !*head)
		return ;
	current = *head;
	while (current)
	{
		next = current->next;
		if (current->word)
			free(current->word);
		free(current);
		current = next;
	}
	*head = NULL;
}

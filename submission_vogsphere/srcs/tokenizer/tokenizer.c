/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:35:04 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:49:56 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Scan the input string and build the token list by dispatching to
** append_operator or append_word. Called by tokenize.
*/
static int	process_tokens(t_token **head, char **line)
{
	while (**line)
	{
		if (is_space(**line))
			consume_space(line);
		else if (is_metachar(**line))
		{
			if (!append_operator(head, line))
				return (0);
		}
		else
		{
			if (!append_word(head, line))
				return (0);
		}
	}
	return (1);
}

/*
** Append a TK_EOF sentinel to the list.
** Frees the entire list and returns NULL on allocation failure.
*/
static t_token	*append_eof(t_token **head)
{
	t_token	*eof_token;

	eof_token = token_new(NULL, TK_EOF);
	if (!eof_token)
	{
		token_free(head);
		return (NULL);
	}
	token_add_back(head, eof_token);
	return (*head);
}

/*
** Entry point for tokenization. Checks for unclosed quotes, builds the token
** list, appends a TK_EOF sentinel, and validates syntax.
** Returns the token list head, or NULL on error.
*/
t_token	*tokenize(char *line)
{
	t_token	*head;

	head = NULL;
	if (line == NULL)
		return (NULL);
	if (has_unclosed_quote(line))
	{
		ft_putendl_fd("minishell: syntax error: unclosed quote", STDERR_FILENO);
		return (NULL);
	}
	if (!process_tokens(&head, &line))
	{
		token_free(&head);
		return (NULL);
	}
	if (append_eof(&head) == NULL)
		return (NULL);
	if (ft_check_syntax(head) == -1)
	{
		token_free(&head);
		return (NULL);
	}
	return (head);
}

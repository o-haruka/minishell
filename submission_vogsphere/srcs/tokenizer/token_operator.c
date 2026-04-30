/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:34:25 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:45:27 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Create a token with the given string and kind and append it to head.
** Returns 1 on success, 0 on allocation failure.
*/
static int	add_operator_token(t_token **head, char *operator_str,
		t_token_kind kind)
{
	t_token	*new_token;

	if (!operator_str)
		return (0);
	new_token = token_new(operator_str, kind);
	if (!new_token)
	{
		free(operator_str);
		return (0);
	}
	token_add_back(head, new_token);
	return (1);
}

/*
** Consume '|' from *line and add a TK_PIPE token to head.
*/
static int	append_pipe(t_token **head, char **line)
{
	char	*operator_str;

	(*line)++;
	operator_str = ft_strdup("|");
	if (operator_str == NULL)
		return (0);
	return (add_operator_token(head, operator_str, TK_PIPE));
}

/*
** Consume '<' or '<<' from *line and add TK_REDIRECT_IN or TK_HEREDOC.
*/
static int	append_redirect_in(t_token **head, char **line)
{
	char	*operator_str;

	if ((*line)[1] == '<')
	{
		(*line) += 2;
		operator_str = ft_strdup("<<");
		if (operator_str == NULL)
			return (0);
		return (add_operator_token(head, operator_str, TK_HEREDOC));
	}
	(*line)++;
	operator_str = ft_strdup("<");
	if (operator_str == NULL)
	{
		return (0);
	}
	return (add_operator_token(head, operator_str, TK_REDIRECT_IN));
}

/*
** Consume '>' or '>>' from *line and add TK_REDIRECT_OUT or TK_APPEND.
*/
static int	append_redirect_out(t_token **head, char **line)
{
	char	*operator_str;

	if ((*line)[1] == '>')
	{
		(*line) += 2;
		operator_str = ft_strdup(">>");
		if (operator_str == NULL)
			return (0);
		return (add_operator_token(head, operator_str, TK_APPEND));
	}
	(*line)++;
	operator_str = ft_strdup(">");
	if (operator_str == NULL)
		return (0);
	return (add_operator_token(head, operator_str, TK_REDIRECT_OUT));
}

/*
** Dispatch to append_pipe, append_redirect_out, or append_redirect_in
** based on the current character. Called by process_tokens.
*/
int	append_operator(t_token **head, char **line)
{
	if (**line == '|')
		return (append_pipe(head, line));
	else if (**line == '>')
		return (append_redirect_out(head, line));
	else if (**line == '<')
		return (append_redirect_in(head, line));
	return (1);
}

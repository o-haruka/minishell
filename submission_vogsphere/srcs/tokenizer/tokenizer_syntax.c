/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_syntax.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:34:26 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:45:39 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Print a syntax error and return -1. Shows "newline" for EOF/NULL tokens,
** otherwise shows the actual token string. Called by check_pipe/check_redir.
*/
static int	print_syntax_err(t_token *tok)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `",
		STDERR_FILENO);
	if (tok == NULL || tok->kind == TK_EOF)
		ft_putstr_fd("newline", STDERR_FILENO);
	else
		ft_putstr_fd(tok->word, STDERR_FILENO);
	ft_putendl_fd("'", STDERR_FILENO);
	return (-1);
}

/*
** Verify that a redirection operator is followed by a WORD token.
** Returns -1 on error. Called by ft_check_syntax.
*/
static int	check_redir(t_token *tok)
{
	t_token	*next;

	next = tok->next;
	if (next == NULL || next->kind != TK_WORD)
		return (print_syntax_err(next));
	return (0);
}

/*
** Verify that '|' is not at the start and is followed by a non-pipe token.
** Returns -1 on error. Called by ft_check_syntax.
*/
static int	check_pipe(t_token *prev, t_token *tok)
{
	t_token	*next;

	if (prev == NULL)
		return (print_syntax_err(tok));
	next = tok->next;
	if (next == NULL || next->kind == TK_EOF || next->kind == TK_PIPE)
		return (print_syntax_err(next));
	return (0);
}

/*
** Walk the token list and validate pipe and redirection syntax.
** Returns 0 on success, -1 on error. Called by tokenize.
*/
int	ft_check_syntax(t_token *tokens)
{
	t_token	*cur;
	t_token	*prev;

	prev = NULL;
	cur = tokens;
	while (cur != NULL && cur->kind != TK_EOF)
	{
		if (cur->kind == TK_PIPE)
		{
			if (check_pipe(prev, cur) == -1)
				return (-1);
		}
		else if (cur->kind == TK_REDIRECT_IN || cur->kind == TK_REDIRECT_OUT
			|| cur->kind == TK_APPEND || cur->kind == TK_HEREDOC)
		{
			if (check_redir(cur) == -1)
				return (-1);
		}
		prev = cur;
		cur = cur->next;
	}
	return (0);
}

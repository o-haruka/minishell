/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:34:58 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:45:42 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_space(char c)
{
	return (c == ' ' || c == '\t');
}

/*
** Return true if c is a shell metacharacter (|, <, >).
*/
bool	is_metachar(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

void	consume_space(char **line)
{
	while (**line && is_space(**line))
		(*line)++;
}

/*
** Return 1 if line contains an unmatched single- or double-quote, 0 otherwise.
** Called by tokenize before building the token list.
*/
int	has_unclosed_quote(char *line)
{
	char	quote;

	quote = 0;
	while (*line)
	{
		if (quote)
		{
			if (*line == quote)
				quote = 0;
		}
		else
		{
			if (*line == '\'' || *line == '"')
				quote = *line;
		}
		line++;
	}
	return (quote != 0);
}

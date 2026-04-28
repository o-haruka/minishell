/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:34:30 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:50:25 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// ポインタを進める処理
// クォート内のスペースやメタ文字を単語の区切りとして扱わないために、クォートの開閉状態を追跡する。
// 例: "hello world" → スペースを無視して1トークンにする
static void	advance_word_ptr(char **line)
{
	char	quote;

	quote = 0;
	while (**line)
	{
		if (quote && **line == quote)
			quote = 0;
		else if (!quote)
		{
			if (is_space(**line) || is_metachar(**line))
				break ;
			if (**line == '\'' || **line == '\"')
				quote = **line;
		}
		(*line)++;
	}
}

// 単語の終了位置までポインタを進め、トークンを作成する
int	append_word(t_token **head, char **line)
{
	char	*start;
	char	*word_str;
	t_token	*new_token;

	start = *line;
	advance_word_ptr(line);
	word_str = ft_substr(start, 0, *line - start);
	if (!word_str)
		return (0);
	new_token = token_new(word_str, TK_WORD);
	if (!new_token)
	{
		free(word_str);
		return (0);
	}
	token_add_back(head, new_token);
	return (1);
}

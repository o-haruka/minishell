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

// メタ文字 (区切り文字) かどうか
// シェルにとって特別な意味を持つ文字(必須課題): | < >
//! | < >だけでいいのか？
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
** クォート（' または "）が正しく閉じられているか確認する。
** 閉じられていない場合は 1 を返す。
** 例:
**   echo 'hello'  → 0（正常）
**   echo 'hello   → 1（エラー: シングルクォートが閉じていない）
**   echo "hello  → 1（エラー: ダブルクォートが閉じていない）
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

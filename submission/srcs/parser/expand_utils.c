/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 07:49:36 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/11 12:43:34 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"

/*
** t_env リストを走査し、key が一致する value を返す。
** 見つからなければ NULL を返す。
*/
char	*ft_find_env(t_env *env, char *key, int len)
{
	while (env)
	{
		// key の長さも一致させることで部分一致を防ぐ
		if (ft_strncmp(env->key, key, len) == 0
			&& (int)ft_strlen(env->key) == len)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** '$' の直後から変数名として有効な文字（英数字・_）が何文字続くかを返す。
*/
int	ft_get_var_len(char *s)
{
	int	len;

	len = 0;
	while (s[len] && (ft_isalnum(s[len]) || s[len] == '_'))
		len++;
	return (len);
}

/*
** str[*i] の1文字を result の末尾に連結して返す。
** 連結前の result は free する。処理後に *i を1つ進める。
*/
char	*ft_append_char(char *result, char *str, int *i)
{
	char	*tmp;
	char	*joined;

	tmp = ft_substr(str, *i, 1); // str[*i] の1文字だけを切り出す
	joined = ft_strjoin(result, tmp);
	free(result);
	free(tmp);
	*i += 1;
	return (joined);
}

/*
** 展開結果 part を result の末尾に連結して返す。
** 連結前の result と part は両方 free する。
*/
char	*ft_append_expanded(char *result, char *part)
{
	char	*joined;

	joined = ft_strjoin(result, part);
	free(result);
	free(part);
	return (joined);
}

/*
** '$' 1つ分を処理して、対応する値の文字列を返す。
** $? → last_status を文字列化して返す
** $VAR → env リストを検索して value を返す
** $ 単体 → "$" をそのまま返す
** 未定義変数 → "" を返す
*/
char	*ft_get_dollar_value(char *str, int *i, t_shell *shell)
{
	int		len;
	char	*value;

	*i += 1; // '$' を読み飛ばす
	if (str[*i] == '?')
	{
		*i += 1; // '?' を読み飛ばす
		return (ft_itoa(shell->last_status)); // 終了コードを文字列に変換して返す
	}
	len = ft_get_var_len(str + *i); // '$' 直後の変数名が何文字続くか調べる
	if (len == 0)
		return (ft_strdup("$")); // 変数名がない場合は '$' をそのまま返す
	value = ft_find_env(shell->env, str + *i, len); // env リストから値を検索
	*i += len; // 変数名の分だけ読み込み位置を進める
	if (!value)
		return (ft_strdup("")); // 未定義変数は空文字列を返す
	return (ft_strdup(value)); // 見つかった値をコピーして返す
}

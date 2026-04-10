/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 08:33:40 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/09 08:43:05 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** '$' 1つ分を処理して、対応する値の文字列を返す。
** $? → last_status を文字列化して返す
** $VAR → env リストを検索して value を返す
** $ 単体 → "$" をそのまま返す
** 未定義変数 → "" を返す
*/
static char	*ft_get_dollar_value(char *str, int *i, t_shell *shell)
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

/*
** 文字列 str を1文字ずつ走査する。
** '$' が来たら ft_get_dollar_value で展開した値を連結する。
** それ以外の文字は ft_append_char でそのまま連結する。
** in_sq が true（シングルクォート内）のときは展開しない。
*/
static char	*ft_expand_str(char *str, t_shell *shell, bool in_sq)
{
	char	*result;
	char	*part;
	int		i;

	result = ft_strdup(""); // 空文字列からスタートして1文字ずつ足す
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && !in_sq)
		{
			// '$' を発見 → 展開した値を result に連結する
			part = ft_get_dollar_value(str, &i, shell);
			result = ft_append_expanded(result, part); // result と part を free して新文字列を返す
		}
		else
			result = ft_append_char(result, str, &i); // 通常文字をそのまま result に追加
	}
	return (result);
}

/*
** コマンドリスト全体を走査し、各コマンドの args[] を展開する。
** 展開前の文字列を free し、展開後の文字列で上書きする。
**
** コマンドリスト：パイプで繋がったコマンドの連結リスト
**   例) "ls | grep foo" → [cmd: ls] → [cmd: grep foo] → NULL
** コマンド：args[] に {"ls", "-la", NULL} のような引数配列を持つ構造体
*/
void	ft_expand_args(t_shell *shell)
{
	t_cmd	*cmd;
	char	*expanded;
	int		i;

	cmd = shell->cmds;
	while (cmd) // パイプで繋がった全コマンドをループ
	{
		i = 0;
		while (cmd->args[i]) // このコマンドの全引数をループ
		{
			if (!ft_strchr(cmd->args[i], '$')) // '$' が無い引数は展開不要なので、そのまま次へ進む
				i++;
			else
			{
				expanded = ft_expand_str(cmd->args[i], shell, false); // '$' がある引数は展開
				free(cmd->args[i]); // 展開前の古い文字列を解放
				cmd->args[i] = expanded; // 展開後の文字列で上書き
				i++;
			}
		}
		cmd = cmd->next; // 次のコマンドへ（パイプの右側）
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 08:33:40 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/07 08:46:59 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** str[*i] が '$' のとき呼ばれる。
** '$?' なら last_status を文字列化して返す。
** '$VAR' なら env リストを検索して value を返す。
** （未定義変数の場合は "" を、'$' 単体の場合は "$" を返す）
** 展開後、*i を読み終えた文字数分だけ進める。
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
** この関数だけが外部（parser.c など）から呼ばれる。
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
			expanded = ft_expand_str(cmd->args[i], shell, false);
			free(cmd->args[i]); // 展開前の古い文字列を解放
			cmd->args[i] = expanded; // 展開後の文字列で上書き
			i++;
		}
		cmd = cmd->next; // 次のコマンドへ（パイプの右側）
	}
}



























/*
次はexpand_utils の実装
*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:36 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 17:11:15 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 「コマンドの引数（args）を正しく解析する」
// `-n` オプション　改行を消す

/*
echo 実装のポイント：-n オプションの罠
echo は単に引数を表示するだけに見えるが、本家のBashには「改行を抑制する -n オプション」に関する細かいルールがある。

基本: echo hello → hello\n（改行あり）
オプション: echo -n hello → hello（改行なし）
罠1（連続）: echo -nnnnn hello → これも -n とみなされ、改行なし。
罠2（複数）: echo -n -n -n hello → これもすべてオプションとみなされ、改行なし。
罠3（引数扱い）: echo -nhello → これはオプションではなく、ただの文字列として出力。
*/

/*-----テストケース-----
minishell> echo hello world
→ hello world（改行あり）が出るか？

minishell> echo -n hello
→ hello の後にすぐプロンプトが戻ってくるか？

minishell> echo -n -n -nnnn hello
→ ちゃんとオプションとして無視され、hello だけが出るか？

minishell> echo -nnna hello
→ オプションではなく文字列として扱われる

minishell> echo "-n" hello
→ Expander（展開処理） が正しく動いていれば、クォートが外れて -n として判定される
*/

// オプション "-n" かどうかを判定するヘルパー関数
// "-nnnn" のような形式も許容するBashの仕様に対応
static bool	is_n_option(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (false);
	i = 2;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

int	ft_echo(t_cmd *cmd)
{
	int		i;
	bool	n_flag;

	n_flag = false;
	i = 1;
	while (cmd->args[i] && is_n_option(cmd->args[i]))
	{
		n_flag = true;
		i++;
	}
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], STDOUT_FILENO);
		if (cmd->args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (!n_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:29:53 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:44:08 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** 全環境変数を "declare -x KEY="VALUE"" 形式で表示する。
** value が NULL のものは "declare -x KEY" と表示する。
** 出力例:
**   declare -x HOME="/home/hkuninag"
**   declare -x PATH="/usr/bin:/bin"
**   declare -x NAME        ← value が NULL の場合（値なしで登録された変数）
*/
static void	ft_print_export(t_env *env)
{
	while (env)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(env->key, STDOUT_FILENO);
		if (env->value != NULL)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(env->value, STDOUT_FILENO);
			ft_putchar_fd('"', STDOUT_FILENO);
		}
		ft_putchar_fd('\n', STDOUT_FILENO);
		env = env->next;
	}
}

/*
** 環境変数名として有効かチェックする。
** 戻り値: 1（有効）/ 0（無効）
**
** 有効な環境変数名のルール：
**   - 1文字以上であること
**   - 先頭は英字（a-z, A-Z）またはアンダースコア（_）
**   - 2文字目以降は英数字（a-z, A-Z, 0-9）またはアンダースコア（_）
**
** 例:
**   "NAME"   → 有効
**   "_VAR"   → 有効
**   ""       → 無効（空文字）
**   "=VALUE" → 無効（先頭が =）
**   "1NAME"  → 無効（先頭が数字）
**   "NA-ME"  → 無効（ハイフンは使えない）
*/
static int	ft_is_valid_key(char *key)
{
	int	i;

	if (!key || key[0] == '\0')
		return (0);
	if (!ft_isalpha(key[0]) && key[0] != '_')
		return (0);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/*
** '=' を含む引数 "KEY=VALUE" を解析して env に登録する。
** key を ft_substr で切り出し、有効性を確認してから update_env_value を呼ぶ。
** 戻り値: 0（成功）/ 1（失敗）
*/
static int	ft_export_with_eq(t_shell *shell, char *arg, char *eq)
{
	char	*key;
	char	*value;

	key = ft_substr(arg, 0, eq - arg);
	value = eq + 1;
	if (!key)
		return (1);
	if (!ft_is_valid_key(key))
	{
		print_error_msg("export", arg, "not a valid identifier");
		free(key);
		return (1);
	}
	if (update_env_value(&shell->env, key, value) != 0)
	{
		ft_putendl_fd("minishell: export: malloc failed", STDERR_FILENO);
		free(key);
		return (1);
	}
	free(key);
	return (0);
}

/*
** 引数1つを解析して env リストに追加・更新する。
** '=' なし → key だけ登録、'=' あり → ft_export_with_eq に委譲する。
** 戻り値: 0（成功）/ 1（失敗）
*/
static int	ft_export_var(t_shell *shell, char *arg)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq != NULL)
		return (ft_export_with_eq(shell, arg, eq));
	if (!ft_is_valid_key(arg))
	{
		print_error_msg("export", arg, "not a valid identifier");
		return (1);
	}
	return (update_env_value(&shell->env, arg, NULL));
}

/*
** export ビルトインの実装。
** 引数なし     → 全変数を "declare -x" 形式で一覧表示
** 引数あり     → 各引数を解析して env リストに追加・更新
**
** 例:
**   export               → 全環境変数を表示
**   export NAME=tanaka   → NAME=tanaka を登録
**   export A=1 B=2       → A と B をまとめて登録
**   export INVALID=      → key="INVALID", value="" として登録（空値はOK）
*/
int	ft_export(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	has_error;

	if (!cmd->args[1])
	{
		ft_print_export(shell->env);
		return (0);
	}
	has_error = 0;
	i = 1;
	while (cmd->args[i])
	{
		if (ft_export_var(shell, cmd->args[i]) != 0)
			has_error = 1;
		i++;
	}
	return (has_error);
}

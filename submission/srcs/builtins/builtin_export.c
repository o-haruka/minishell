/*hkuninag担当*/
#include "minishell.h"
#include "libft.h"

/*
** 全環境変数を "declare -x KEY="VALUE"" 形式で表示する。
** value が NULL のものは "declare -x KEY" と表示する。
*/
static void	ft_print_export(t_env *env)
{
	while (env)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(env->key, STDOUT_FILENO);
		if (env->value != NULL) // value がある場合だけ ="VALUE" を表示
		{
			ft_putstr_fd("=\"", STDOUT_FILENO); // =" を出力
			ft_putstr_fd(env->value, STDOUT_FILENO);
			ft_putchar_fd('"', STDOUT_FILENO); // 閉じ " を出力
		}
		ft_putchar_fd('\n', STDOUT_FILENO); // 改行
		env = env->next;
	}
}

/*
** 引数を解析して env リストに追加・更新する。
** "NAME=hkuninag" → key="NAME", value="hkuninag" に分割して登録。
** "NAME"        → value なしで key だけ登録。
*/
static void	ft_export_var(t_shell *shell, char *arg)
{
	char	*eq;
	char	*key;
	char	*value;

	eq = ft_strchr(arg, '=');
	if (eq == NULL)
	{
		// '=' がない → key だけ登録（value は NULL）
		update_env_value(&shell->env, arg, NULL);
		return ;
	}
	key = ft_substr(arg, 0, eq - arg); // '=' より前を key に
	value = eq + 1;
	if (!key)
		return ;
	update_env_value(&shell->env, key, value); // env リストを更新
	free(key); // ft_substr で malloc されたので free する
}

/*
** export ビルトインの実装。
** 引数なし     → 全変数を一覧表示
** 引数あり     → 各引数を解析して env リストに追加・更新
*/
int	ft_export(t_cmd *cmd, t_shell *shell)
{
	int	i;

	if (!cmd->args[1]) // 引数なしの場合
	{
		ft_print_export(shell->env); // 全変数を表示して終了
		return (0);
	}
	i = 1;					// args[0] は "export" なので1から始める
	while (cmd->args[i])	// NULL が来るまでループ
	{
		ft_export_var(shell, cmd->args[i]); // 各引数を解析して登録
		i++;
	}
	return (0);
}

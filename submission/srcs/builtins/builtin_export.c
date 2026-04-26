/*hkuninag担当*/
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
		ft_putstr_fd("declare -x ", STDOUT_FILENO); // bash と同じ形式のプレフィックス
		ft_putstr_fd(env->key, STDOUT_FILENO); // キー名を出力（例: HOME）
		if (env->value != NULL) // value がある場合だけ ="VALUE" を表示
		{
			ft_putstr_fd("=\"", STDOUT_FILENO); // =" を出力
			ft_putstr_fd(env->value, STDOUT_FILENO); // 値を出力（例: /home/hkuninag）
			ft_putchar_fd('"', STDOUT_FILENO); // 閉じ " を出力
		}
		ft_putchar_fd('\n', STDOUT_FILENO); // 1変数ごとに改行
		env = env->next; // 次のノードへ進む
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

	if (!key || key[0] == '\0')      // 空文字は無効
		return (0);
	if (!ft_isalpha(key[0]) && key[0] != '_') // 先頭が英字か _ でなければ無効
		return (0);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_') // 2文字目以降のチェック
			return (0);
		i++;
	}
	return (1); // 全チェック通過 → 有効
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
		ft_putstr_fd("minishell: export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
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
		ft_putstr_fd("minishell: export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
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

	if (!cmd->args[1]) // 引数なしの場合 → 一覧表示して終了
	{
		ft_print_export(shell->env);
		return (0);
	}
	has_error = 0; // 最初は「成功」としておく
	i = 1;					// args[0] は "export" なので1から始める
	while (cmd->args[i])	// NULL が来るまで全引数をループ
	{
		if (ft_export_var(shell, cmd->args[i]) != 0)
			has_error = 1; // 1つでも失敗したら has_error を 1 にする（残りは処理し続ける）
		i++;
	}
	return (has_error); // 全引数を処理してから最終的な結果を返す
}

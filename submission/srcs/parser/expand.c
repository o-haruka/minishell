#include "minishell.h"
#include "libft.h"

/* ── 処理の流れ（上が入口、下が詳細） ────────────────────────
**   ft_expand_args          コマンドリスト全体のエントリポイント
**     ft_expand_str         1つの文字列を展開する
**       ft_toggle_quote     クォートの開始/終了をトグルする
**       ft_handle_dollar    '$' を処理する
**         ft_get_dollar_value  '$' の展開値を決定する
** ──────────────────────────────────────────────────────────── */

static char	*ft_expand_str(char *str, t_shell *shell);
static void	ft_toggle_quote(char c, bool *in_sq, bool *in_dq, int *i);
static int	ft_handle_dollar(char **res, char *str, int *i, t_shell *shell);
static char	*ft_get_dollar_value(char *str, int *i, t_shell *shell);

/*
** エントリポイント：shell->cmds の全コマンドを走査し、
** 各 args[] をクォート除去・環境変数展開して上書きする。
**
** 例）ユーザーが echo "$HOME" 'hello' と入力した場合
**     args = {"echo", "\"$HOME\"", "'hello'", NULL}
**     展開後 = {"echo", "/Users/hk", "hello",   NULL}
*/
void	ft_expand_args(t_shell *shell)
{
	t_cmd	*cmd;
	char	*expanded;
	int		i;

	cmd = shell->cmds;
	while (cmd) // パイプで繋がった各コマンドを順番に処理
	{
		i = 0;
		// cmd->args の中身例: {"echo", "\"$HOME\"", "'hello'", NULL}
		//   args[0] = "echo"      コマンド名
		//   args[1] = "\"$HOME\"" 展開が必要な引数
		//   args[2] = "'hello'"   クォート除去が必要な引数
		//   args[3] = NULL        終端（NULL が番兵）
		while (cmd->args[i]) // NULL に達するまで各引数を処理
		{
			expanded = ft_expand_str(cmd->args[i], shell);
			if (!expanded)
			{
				shell->last_status = 1;
				return ;
			}
			free(cmd->args[i]);      // 展開前の文字列を解放
			cmd->args[i] = expanded; // 展開後の文字列で上書き
			i++;
		}
		cmd = cmd->next; // 次のコマンド（パイプの右側）へ
	}
}

/*
** 文字列 str を1文字ずつ走査し、展開済みの新しい文字列を返す。
**
** 例）"$HOME/doc"  → "/Users/hk/doc"  （$HOME を展開）
**    "'$HOME'"    → "$HOME"           （' ' 内は展開しない）
**    "\"$HOME\""  → "/Users/hk"       （" " 内の $ は展開する）
**    "hello"      → "hello"           （変化なし）
**
** クォート文字（' や "）自体は結果に含まれない。
*/
static char	*ft_expand_str(char *str, t_shell *shell)
{
	char	*result;
	int		i;
	bool	in_sq;
	bool	in_dq;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	in_sq = false;
	in_dq = false;
	while (str[i])
	{
		if ((str[i] == '\'' && !in_dq) // 【判定①】クォートの開閉か？ → " の中の ' は開閉扱いしない（"it's" の例）
			|| (str[i] == '"' && !in_sq)) //                          → ' の中の " も開閉扱いしない（'say "hi"' の例）
			ft_toggle_quote(str[i], &in_sq, &in_dq, &i); // クォート状態を更新（文字自体は追加しない）
		else if (str[i] != '$' || in_sq) // 【判定②】展開しない文字か？（$ 以外、またはシングルクォート内の $）
			result = ft_append_char(result, str, &i); // → そのまま追加
		else if (ft_handle_dollar(&result, str, &i, shell) != 0) // 【判定③】展開すべき $ が確定（シングルクォート外の $）
			return (NULL); // → 展開失敗
		if (!result)
			return (NULL);
	}
	return (result);
}

/*
** クォート文字 c に応じて in_sq / in_dq をトグルし、i を1進める。
** クォート文字自体は result に追加しない（呼び出し元で追加処理をしない）。
**
** 例）'hello' を処理するとき
**   ' で in_sq が false → true になる（シングルクォート開始）
**   h e l l o はそのまま result に追加される
**   ' で in_sq が true → false になる（シングルクォート終了）
*/
static void	ft_toggle_quote(char c, bool *in_sq, bool *in_dq, int *i)
{
	if (c == '\'')
		*in_sq = !*in_sq;
	else
		*in_dq = !*in_dq;
	*i += 1;
}

/*
** '$' 1文字分を処理し、*res を展開結果で更新する。
** 戻り値: 0=成功、-1=失敗（失敗時は *res を解放して NULL にする）
**
** res が char **（ダブルポインタ）な理由:
**   ft_append_expanded は新しいアドレスを返すため、
**   呼び出し元の result 変数自体を更新する必要があるから。
**   char * のままだと呼び出し元の result を書き換えられない。
*/
static int	ft_handle_dollar(char **res, char *str, int *i, t_shell *shell)
{
	char	*part;

	part = ft_get_dollar_value(str, i, shell);
	if (!part)
	{
		free(*res);
		*res = NULL;
		return (-1);
	}
	*res = ft_append_expanded(*res, part);
	if (!*res)
		return (-1);
	return (0);
}

/*
** '$' 1つ分を処理して、対応する値の文字列を返す。
** 戻り値は必ず ft_strdup で新規確保した文字列（呼び出し元が free する）。
**
** $?       → 直前のコマンドの終了コード（例: "0", "127"）
** $HOME    → env リストから検索した値（例: "/Users/hk"）
** $ 単体   → '$' をそのまま返す（$ の直後が変数名でない場合）
** 未定義変数 → "" を返す（エラーにはしない）
*/
static char	*ft_get_dollar_value(char *str, int *i, t_shell *shell)
{
	int		len;
	char	*value;

	*i += 1; // '$' 自体を読み飛ばして次の文字へ進む
	if (str[*i] == '?') // $? か判定
	{
		*i += 1; // '?' を読み飛ばす
		return (ft_itoa(shell->last_status)); // 終了コード（整数）を文字列に変換して返す
	}
	len = ft_get_var_len(str + *i); // '$' の直後から変数名が何文字続くか調べる
	if (len == 0) // 変数名が0文字 = '$' の直後が英数字・_ でない（例: "$ " や "$/"）
		return (ft_strdup("$")); // '$' をただの文字として返す（NULL チェックは呼び出し元に任せる）
	value = ft_find_env(shell->env, str + *i, len); // 変数名で環境変数リストを検索
	*i += len; // 変数名の分だけ読み込み位置を進める
	if (!value) // 環境変数が未定義だった
		return (ft_strdup("")); // 未定義変数は空文字列を返す（エラーにはしない）
	return (ft_strdup(value)); // 見つかった値をコピーして返す（呼び出し元が free する）
}

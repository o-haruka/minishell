#include "minishell.h"
#include "libft.h"

/*
** 文字列 str を1文字ずつ走査し、展開済みの新しい文字列を返す。
** クォート状態を追跡しながら走査する。
** シングルクォート内 → $ を展開しない
** ダブルクォート内  → $ のみ展開する
** クォート文字自体は result に追加せず除去する
*/
static char	*ft_expand_str(char *str, t_shell *shell, bool in_sq)
{
	char	*result;
	char	*part;
	int		i;
	bool	in_dq;

	result = ft_strdup(""); // 空文字列からスタートして1文字ずつ足す
	i = 0;
	in_dq = false;
	while (str[i])
	{
		if (str[i] == '\'' && !in_dq)
		{
			in_sq = !in_sq; // シングルクォートの開閉を切り替える
			i++;
		}
		else if (str[i] == '"' && !in_sq)
		{
			in_dq = !in_dq; // ダブルクォートの開閉を切り替える
			i++;
		}
		else if (str[i] == '$' && !in_sq)
		{
			part = ft_get_dollar_value(str, &i, shell);
			result = ft_append_expanded(result, part);
		}
		else
			result = ft_append_char(result, str, &i); // 通常文字をそのまま result に追加
	}
	return (result);
}

/*
** コマンドリスト全体を走査し、各コマンドの args[] を
** クォート除去・環境変数展開して上書きする。
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
			expanded = ft_expand_str(cmd->args[i], shell, false); // クォート除去＋$展開を全引数に適用
			free(cmd->args[i]); // 展開前の古い文字列を解放
			cmd->args[i] = expanded; // 展開後の文字列で上書き
			i++;
		}
		cmd = cmd->next; // 次のコマンドへ（パイプの右側）
	}
}

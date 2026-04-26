/*hkuninag担当*/

#include "minishell.h"

/*
** env リストから key が一致するノードを1つ削除する。
** 削除の仕組み：
**   削除前: [prev] → [current] → [next]
**   削除後: [prev] → [next]
**   ※ prev->next を current->next に繋ぎ替えて current を free する
** 先頭ノードが対象の場合は shell->env 自体を次に付け替える。
*/
static void	ft_unset_var(t_shell *shell, char *key)
{
	t_env	*current;
	t_env	*prev;

	prev = NULL;// 1つ前のノード（最初は存在しないのでNULL）
	current = shell->env; // リストの先頭から探し始める
	while (current)
	{
		// key と完全一致するノードか確認
		// +1 で NULL文字まで比較することで部分一致を防ぐ
		// 例: "PATH" と "PATH2" を区別するために必要
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)

		{
			if (prev == NULL)
				// prevがNULL = まだ1つも進んでいない = 先頭ノードが削除対象
				// shell->env 自体を次のノードに付け替える
				shell->env = current->next;
			else
				// prevがNULL以外 = 2番目以降のノードが削除対象
				// 1つ前のノードの next を、削除ノードの次に繋ぎ替える
				prev->next = current->next;
			// ノードが持つメモリを個別に解放する
			// free(current) だけでは 中身の key と value は解放されないので個別に解放する必要あり
			free(current->key); // キー文字列を解放
			free(current->value); // 値文字列を解放
			free(current); // ノード本体を解放
			return ; // 削除したら終了
		}
		prev = current; // 現在のノードを「1つ前」として記録
		current = current->next; // 次のノードへ進む
	}
	// ループを抜けた = key が見つからなかった → 何もしない
	// bash の仕様: 存在しない変数を unset してもエラーにならない
}

/*
** unset ビルトインの実装。
** 引数を複数受け取り、それぞれを env リストから削除する。
**
** 例) unset NAME USER PATH
**   → args[0]="unset", args[1]="NAME", args[2]="USER", args[3]="PATH"
**   → NAME, USER, PATH を順番に env リストから削除する
*/
int	ft_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;

	i = 1; // args[0] は "unset" なので1から始める
	while (cmd->args[i]) // NULL が来るまでループ（引数の終わりを検出）
	{
		ft_unset_var(shell, cmd->args[i]); // 各引数をリストから削除
		i++;
	}
	return (0); // unset は常に成功を返す
}

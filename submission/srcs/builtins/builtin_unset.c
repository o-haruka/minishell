/*hkuninag担当*/

#include "minishell.h"
#include "libft.h"

/*
** env リストから key が一致するノードを1つ削除する。
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
		// key と完全一致するノードか確認（+1でNULL文字まで比較し部分一致を防ぐ）
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
			free(current->key); // キー文字列を解放
			free(current->value); // 値文字列を解放
			free(current); // ノード本体を解放
			return ; // 削除したら終了
		}
		prev = current; // 現在のノードを「1つ前」として記録
		current = current->next; // 次のノードへ進む
	}
}

/*
** unset ビルトインの実装。
** 引数を複数受け取り、それぞれを env リストから削除する。
*/
int	ft_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;

	i = 1; // args[0] は "unset" なので1から始める
	while (cmd->args[i]) // NULL が来るまでループ
	{
		ft_unset_var(shell, cmd->args[i]); // 各引数をリストから削除
		i++;
	}
	return (0);
}

#include "minishell.h"
#include "libft.h"

/*
** "KEY=VALUE" 形式の文字列から t_env を1つ作って返す。
** key と value はそれぞれ ft_strdup でコピーして保持する。
*/
static t_env	*ft_new_env(char *entry)
{
	t_env	*node;
	char	*eq;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	eq = ft_strchr(entry, '='); // '=' の位置を探す
	if (!eq)
	{
		free(node);
		return (NULL);
	}
	node->key = ft_substr(entry, 0, eq - entry); // eq - entry で '=' の前の文字数を求める
	node->value = ft_strdup(eq + 1); // eq + 1 で '=' の1つ後ろ（value の先頭）を指す
	node->next = NULL;
	if (!node->key || !node->value)
	{
		free(node->key);
		free(node->value);
		free(node);
		return (NULL);
	}
	return (node);
}

/*
** envp（文字列配列）を走査し、t_env の連結リストに変換して返す。
** 例) envp = {"HOME=/home/rex", "USER=rex", NULL}
**     → [key=HOME, value=/home/rex] → [key=USER, value=rex] → NULL
** main.c の初期化処理から呼ばれる。
*/
t_env	*init_env(char **envp)
{
	t_env	*head;
	t_env	*tail;
	t_env	*node;
	int		i;

	head = NULL;
	tail = NULL; // tail を持つことで、末尾追加を O(1) で行える（毎回リストを辿らなくて済む）
	i = 0;
	while (envp[i])
	{
		node = ft_new_env(envp[i]);
		if (!node)
		{
			free_env(head); // 途中失敗 → それまでのリストを全解放
			return (NULL);  // NULL を返して呼び出し元にエラーを伝える
		}
		if (!head)
			head = node; // 最初のノードをリストの先頭に設定
		else
			tail->next = node; // tail が常に末尾を指しているので直接連結できる
		tail = node; // tail を新しい末尾に更新
		i++;
	}
	return (head);
}

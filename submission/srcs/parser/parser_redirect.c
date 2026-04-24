#include "libft.h"
#include "minishell.h"

#define REDIR_OK      1   // 正常終了
#define REDIR_ERR     0   // メモリ確保などの通常エラー
#define REDIR_SYNTAX -1   // 構文エラー

// --------------------------------------------------------- 
// 2. リダイレクト構造体(t_redir)の初期化関数
// ---------------------------------------------------------
static t_redir	*init_redir_struct(void)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
    *redir = (t_redir){0, NULL, NULL}; //初期化
	return (redir);
}

// ---------------------------------------------------------
// 3. コマンドのリダイレクトリストに新しい要素を追加する関数
// ---------------------------------------------------------
static void	add_redir_to_list(t_cmd *cmd, t_redir *new_redir)
{
	t_redir	*last;

	if (cmd->redirs == NULL)
		cmd->redirs = new_redir;
	else
	{
		last = cmd->redirs;
		while (last->next != NULL)
			last = last->next;
		last->next = new_redir;
	}
}

// t_redir 構造体を作って繋ぐ
// ---------------------------------------------------------
// 1. リダイレクトトークンを解析し、リストに追加する関数
// ---------------------------------------------------------
int handle_redirection(t_cmd *cmd, t_token **current){
	
    t_redir	*new_redir;

    // 1. リダイレクト構造体の作成(初期化関数)
	new_redir = init_redir_struct();
	if (!new_redir)
		return (free_cmd(cmd), REDIR_ERR);
    new_redir->kind = (*current)->kind; // "<" や ">" などの種類を保存

	// 2. ポインタを進めてターゲット（ファイル名）を見る
    *current = (*current)->next;
	// 【重要】ここで「EOF」の恩恵を受けます！
    if (*current != NULL && (*current)->kind == TK_WORD)
    {
        // ファイル名を自分専用にコピー！
        new_redir->file = ft_strdup((*current)->word);
        if (!new_redir->file)
            return(free(new_redir), free_cmd(cmd), REDIR_ERR);
        // ファイル名も読み終わったので、さらに次へ進める
        *current = (*current)->next;
    }
    else // 次がファイル名じゃなかった（EOFやパイプだった）場合
    {
		ft_putendl_fd("minishell: syntax error near unexpected token", 2);
        return (free(new_redir), free_cmd(cmd), REDIR_SYNTAX);
    }
	// 3. cmd->redirs リストの末尾に繋ぐ
    add_redir_to_list(cmd, new_redir);
    return (REDIR_OK);
}

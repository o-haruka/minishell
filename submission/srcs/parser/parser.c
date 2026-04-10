#include "libft.h"
#include "minishell.h"

int count_words(t_token *current)
{
    int count = 0;

    // パイプかEOFが来るまでループ
    while (current != NULL && current->kind != TK_EOF && current->kind != TK_PIPE)
    {
        if (current->kind == TK_WORD)
        {
            count++;                 // コマンドや引数なのでカウントアップ
            current = current->next; // 次へ進む
        }
        // リダイレクト関連の記号だった場合
        else if (current->kind == TK_REDIRECT_IN || current->kind == TK_REDIRECT_OUT ||
                 current->kind == TK_APPEND || current->kind == TK_HEREDOC)
        {
            current = current->next; // 1. まずリダイレクト記号を読み飛ばす
            // 2. 次のトークンがファイル名（TK_WORD）なら、それもカウントせずに読み飛ばす
            if (current != NULL && current->kind == TK_WORD)
            {
                current = current->next;
            }
        }
        else
            current = current->next; // 念のため、予期せぬトークンも飛ばす
    }
    return (count);
}

// ---------------------------------------------------------
// 2. パイプで区切られた「1つのコマンド」を組み立てる関数
// ---------------------------------------------------------
t_cmd	*parse_command(t_token **current)
{
	t_cmd	*cmd;
	int num_words;
    int i;

	// 1. 新しいコマンド構造体を確保
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	// TODO 初期化 (libftのft_bzeroなどを使うと便利) → ft_bzero(cmd, sizeof(t_cmd));
	// TODO 初期化　*cmd = (t_cmd){0};でもOKでは？
	// ↓だとわかりやすい
	// cmd->args = NULL;
	// cmd->redirs = NULL;
	// cmd->next = NULL;
	//---
	// \*cmd = (t_cmd){0};
	*cmd = (t_cmd){NULL, NULL, NULL}; //これがわかりやすい
	// \*cmd = (t_cmd){.args = NULL, .redirs = NULL, .next = NULL}; //可読性は上がるが長い

	// 2. 引数の数を数えて、二次元配列を malloc する
    // \*current を渡すことで、現在の位置からパイプまでの単語数を取得します
	num_words = count_words(*current);
    cmd->args = malloc(sizeof(char *) * (num_words + 1)); // +1 は終端の NULL のため
    if (!cmd->args)
    {
        free(cmd);
        return (NULL);
    }

	// 3. トークンを読み進めながら、配列に単語を詰めていく
	// パイプ (TK_PIPE) か EOF (TK_EOF) が来るまで、1つのコマンドとして読み続ける
    i = 0;
	while (*current != NULL && (*current)->kind != TK_EOF && (*current)->kind != TK_PIPE)
	{
		if ((*current)->kind == TK_WORD)
		{
			// TK_WORD を args配列 に追加する
			// 単語を複製して配列に入れる
            // ※ここで ft_strdup を使うのが超重要！(リストから配列に代入したら、元のリストはfreeするため、strdupでwordを保持する)
            cmd->args[i] = ft_strdup((*current)->word);
            if(!cmd->args[i]){
                free_cmd(cmd);
                return NULL;
            }
            i++;
            // 【重要】ここで現在地（大元のポインタ）を進める！
            *current = (*current)->next;
		}
		else // リダイレクトの場合（TK_REDIRECT_IN, TK_REDIRECT_OUT など）
		{
			// t_redir 構造体を作って繋ぐ
			// 1. リダイレクト構造体の作成
            t_redir *new_redir = malloc(sizeof(t_redir));
            if (!new_redir){
                free_cmd(cmd);
                return (NULL); // ※本当はここで cmd などの free も必要です
            }
            
            new_redir->kind = (*current)->kind; // "<" や ">" などの種類を保存
            new_redir->next = NULL;

			// 2. ポインタを進めてターゲット（ファイル名）を見る
            *current = (*current)->next;

			// 【重要】ここで「EOF」の恩恵を受けます！
            if (*current != NULL && (*current)->kind == TK_WORD)
            {
                // ファイル名を自分専用にコピー！
                new_redir->file = ft_strdup((*current)->word);
                if (!new_redir->file) {
                    free(new_redir);
                    free_cmd(cmd);
                    return NULL;
                }
                // ファイル名も読み終わったので、さらに次へ進める
                *current = (*current)->next;
            }
            else
            {
                // 次がファイル名じゃなかった（EOFやパイプだった）場合
                // printf("minishell: syntax error near unexpected token\n");
				ft_putendl_fd("minishell: syntax error near unexpected token", 2);
                free(new_redir);
                free_cmd(cmd);
                return (NULL); // 構文エラーとしてパースを中断
            }

			// 3. cmd->redirs リストの末尾に繋ぐ
            if (cmd->redirs == NULL)
            {
                cmd->redirs = new_redir; // リストが空なら先頭に
            }
            else
            {
                t_redir *last = cmd->redirs;
                while (last->next != NULL) // 最後尾を探す
                    last = last->next;
                last->next = new_redir; // 末尾に繋ぐ
            }
		}
	}
	cmd->args[i] = NULL; // 最後に NULL を入れて配列を完璧に終端させる
	return (cmd);
}

// ---------------------------------------------------------
// 1. 全体を統括するエントリー関数
// ---------------------------------------------------------
t_cmd *parse(t_token *tokens)
{
    t_cmd *cmd_head = NULL;
    t_cmd *cmd_tail = NULL;

    // 読み進めるための「現在地ポインタ」を別に用意する
    t_token *current_token;
    current_token = tokens; // 現在地ポインタを先頭にセット


    // 構築ループ (EOF に到達するまで全体のパースを続ける)
    while (current_token != NULL && current_token->kind != TK_EOF)
    {
        // 1コマンド分（パイプまで）をパースする。
		// ここで &current_token とアドレスを渡すことで、関数内で現在地が進む！
        // ヘルパー関数には「現在地ポインタのアドレス（ダブルポインタ）」を渡す
        t_cmd *new_cmd = parse_command(&current_token); 
        if(!new_cmd){
            free_cmds_list(cmd_head);
            return NULL;
        }
        
        // ... new_cmd を cmd_head に繋ぐ処理 ...
        // 作ったコマンドをリストの末尾に繋ぐ
		if (cmd_head == NULL)
		{
			cmd_head = new_cmd;
			cmd_tail = new_cmd;
		}
		else
		{
			cmd_tail->next = new_cmd;
			cmd_tail = new_cmd;
		}

		// parse_command を抜けた後、current_token は TK_PIPE か TK_EOF を指しているはず。
		// パイプだったら、1つ進めて次のコマンドのパースに備える。
		if (current_token != NULL && current_token->kind == TK_PIPE)
			current_token = current_token->next;
	}
	return (cmd_head);
    }


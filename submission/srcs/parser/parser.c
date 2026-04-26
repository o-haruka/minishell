/********************************************************
minishellパーサ：コマンド構造体リストを構築する主要関数群
*********************************************************/
#include "minishell.h"

// ---------------------------------------------------------
// 3. コマンド構造体(t_cmd)の初期化関数
// ---------------------------------------------------------
t_cmd *init_cmd_struct(void)
{
    t_cmd *cmd;
    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
		return NULL;

    // ①だとわかりやすい
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->next = NULL;
    // ②簡潔
	// cmd = (t_cmd){0};
    // ③可読性は上がるが長い
    // cmd = (t_cmd){.args = NULL, .redirs = NULL, .next = NULL};
    // ④↓
    // \*cmd = (t_cmd){NULL, NULL, NULL};
    return cmd;
}

// ---------------------------------------------------------
// 4. コマンドの引数数をカウントする関数
// ---------------------------------------------------------
int count_words(t_token *current)
{
    int count;
    count = 0;

    // パイプかEOFが来るまでループ
    while (current != NULL && current->kind != TK_EOF && current->kind != TK_PIPE)
    {
        if (current->kind == TK_WORD)
        {
            count++; // コマンドや引数なのでカウントアップ
            current = current->next; // 次へ進む
        }
        // リダイレクト関連の記号だった場合
        else if (current->kind == TK_REDIRECT_IN || current->kind == TK_REDIRECT_OUT ||
                 current->kind == TK_APPEND || current->kind == TK_HEREDOC)
        {
            current = current->next; // 1. まずリダイレクト記号を読み飛ばす
            // 2. 次のトークンがファイル名（TK_WORD）なら、それもカウントせずに読み飛ばす
            if (current != NULL && current->kind == TK_WORD)
                current = current->next;
        }
        else
            current = current->next; // 念のため、予期せぬトークンも飛ばす
    }
    return (count);
}


// ---------------------------------------------------------
// 5. コマンドトークンを解析し、args/redirsに格納する関数
// ---------------------------------------------------------
static int	process_cmd_tokens(t_cmd *cmd, t_token **current)
{
	int	i;
	int	redir_result;

	i = 0;
	while (*current != NULL && (*current)->kind != TK_EOF
		&& (*current)->kind != TK_PIPE)
	{
		if ((*current)->kind == TK_WORD)
		{
            // TK_WORD を args配列 に追加する (※ここで ft_strdup を使うのが超重要！(リストから配列に代入したら、元のリストはfreeするため、strdupでwordを保持する))
			cmd->args[i] = ft_strdup((*current)->word);
			if (!cmd->args[i])
				return (free_cmd(cmd), 0);
			i++;
			*current = (*current)->next; // 【重要】ここで現在地（大元のポインタ）を進める！
		}
		else // リダイレクトの場合（TK_REDIRECT_IN, TK_REDIRECT_OUT など）
		{
			redir_result = handle_redirection(cmd, current);
			if (redir_result <= 0)
				return (redir_result);
		}
	}
	cmd->args[i] = NULL; // 最後に NULL を入れて配列を完璧に終端させる
	return (1);
}


// ---------------------------------------------------------
// 2. 1コマンド分（パイプ区切り）を組み立てる関数
// ---------------------------------------------------------
t_cmd	*parse_command(t_token **current)
{
	t_cmd	*cmd;
	int num_words;
    int status;

    // 1. 新しいコマンド構造体を確保
    cmd = init_cmd_struct();
    if(cmd == NULL)
		return NULL;

	// 2. 引数の数を数えて、二次元配列を malloc する
    // \*current を渡すことで、現在の位置からパイプまでの単語数を取得します
	num_words = count_words(*current);
	cmd->args = malloc(sizeof(char *) * (num_words + 1)); // +1 は終端の NULL のため
	if (!cmd->args)
		return(free_cmd(cmd), NULL);
    // ゼロ初期化
	ft_memset(cmd->args, 0, sizeof(char *) * (num_words + 1));

	// 3. トークンを読み進めながら、配列に単語を詰めていく
	// パイプ (TK_PIPE) か EOF (TK_EOF) が来るまで、1つのコマンドとして読み続ける
    status = process_cmd_tokens(cmd, current);
    if(status <= 0)
        return NULL;
    return (cmd);
}


// ---------------------------------------------------------
// 1. 全体のパースを統括するエントリー関数
// ---------------------------------------------------------
t_cmd *parse(t_token *tokens)
{
    t_cmd *cmd_head = NULL;
    t_cmd *cmd_tail = NULL;
    t_cmd *new_cmd = NULL;
    t_token *current_token; // 読み進めるための「現在地ポインタ」を別に用意する
    current_token = tokens; // 現在地ポインタを先頭にセット

    // 構築ループ (EOF に到達するまで全体のパースを続ける)
    while (current_token != NULL && current_token->kind != TK_EOF)
    {
        // 1コマンド分（パイプまで）をパースする。
		// ここで &current_token とアドレスを渡すことで、関数内で現在地が進む！
        // ヘルパー関数には「現在地ポインタのアドレス（ダブルポインタ）」を渡す
        new_cmd = parse_command(&current_token); 
        if(!new_cmd)
            return(free_cmds_list(cmd_head), NULL);
        
        // ... new_cmd を cmd_head に繋ぐ処理 ...
        // 作ったコマンドをリストの末尾に繋ぐ
		if (cmd_head == NULL)
			cmd_head = new_cmd;
		else
			cmd_tail->next = new_cmd;
		cmd_tail = new_cmd;

		// parse_command を抜けた後、current_token は TK_PIPE か TK_EOF を指しているはず。
		// パイプだったら、1つ進めて次のコマンドのパースに備える。
		if (current_token != NULL && current_token->kind == TK_PIPE)
			current_token = current_token->next;
	}
	return (cmd_head);
    }


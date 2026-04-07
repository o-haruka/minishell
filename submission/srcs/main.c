#include "libft.h"
#include "minishell.h"
#include <readline/history.h>
#include <readline/readline.h>

// ft_split で確保した二次元配列を解放する関数
void	free_array(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

// [一時的] トークンリストを char **argv に変換する関数
// Parserを作るまでの繋ぎとして、exec_simple_cmd を再利用するために使う
char	**token_list_to_argv(t_token *tokens)
{
	int		count;
	t_token	*tmp;
	char	**argv;
	int		i;

	// 1. 要素数を数える
	count = 0;
	tmp = tokens;
	while (tmp && tmp->kind != TK_EOF) // EOFトークンは含めない
	{
		count++;
		tmp = tmp->next;
	}
	// 2. 配列確保
	argv = malloc(sizeof(char *) * (count + 1));
	if (!argv)
		return (NULL);
	// 3. 文字列をコピー (strdup必須)
	i = 0;
	tmp = tokens;
	while (tmp && tmp->kind != TK_EOF)
	{
		argv[i] = ft_strdup(tmp->word); // ここでコピーしないと後でdouble freeになる
		tmp = tmp->next;
		i++;
	}
	argv[i] = NULL;
	return (argv);
}

/*
** ループ処理を行う関数
** readlineはmallocされた文字列を返すので、使い終わったらfreeが必要です。
** lineがNULLの場合は、Ctrl-D (EOF) が入力されたことを意味します。
*/

// void	minishell_loop(char **environ)　×
void    minishell_loop(t_shell *shell) //○
{
	char *line;
	// char **args; //分割されたコマンド //!shell->cmdsがあるので多分不要

	while (1)
	{
		g_signal = 0; // シグナルステータスをリセット
		line = readline(GREEN "minishell> " RESET); //TODO free必須(readlineはmallocする)
		if (line == NULL){
			//TODO 本家ではここに(break前に)printf("exit\n");が入るらしい
			break ;
		}
		// シグナルが発生した場合は継続
		if (g_signal == SIGINT)
		{
			free(line);
			continue ;
		}
		// 1. 読み込み
		if (*line)
			add_history(line);

		// 2. 字句解析 (Lexer)
		shell->tokens = tokenize(line);
		// [Debug] トークンの中身を見てみる (開発中はこれをコメントアウトして確認)
		// t_token *curr = shell->tokens;
		// while (curr) {
		// 	printf("Token: kind=%d, word=[%s]\n", curr->kind, curr->word);
		// 	curr = curr->next;
		// }

		//TODO  3. Parser : トークンを解析して shell->cmds に変換（今後実装）
		shell->cmds = parse(shell->tokens);
		// [Debug] パース結果（t_cmdリスト）を見たいときだけ有効化
		debug_print_cmds(shell->cmds);
        

		//TODO 3. 実行 (Executor) コマンドを実行し、終了ステータスを更新（今後実装）
		// shell->last_status = execute(shell);

		// ※ テスト用：パーサー実装までの繋ぎ
        /*
        if (shell->tokens && shell->tokens->kind != TK_EOF)
        {
            char **args = token_list_to_argv(shell->tokens);
            // ※ environ が無いので、一時的に NULL を渡すか、main から envp を引っ張ってくる
            execute_command(args, NULL); 
            free_array(args);
        }
        */

		// 4. 後始末
		token_free(&(shell->tokens)); // リスト全体を解放
		free(line);              // readlienで確保したメモリを解放
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell shell; //!全体の状態を持つ構造体
	
	// t_env *env_list; //! 逆に環境変数用の変数が必要。 環境変数の連結リストの先頭ポインタ
	(void)argc;
	(void)argv;

	//! 1. 初期化 (構造体の中身を0クリア) libft使うようにする
    memset(&shell, 0, sizeof(t_shell));
	
	//! 環境変数の取得。envpを解析して、扱いやすい連結リスト(t_env)に変換する
	// TODO: (展開 (Expander)」の実装に取り掛かる直前)に実装
	(void)envp;//後で削除
	// shell.env = init_env(envp);

	shell.last_status = 0;         // 初期ステータスは0

	//2. シグナル初期化
	// setup_signals();

	//3. メインループの開始
	minishell_loop(&shell);

	//4. 終了前の全体メモリ解放
	// free_all_data(env_list);
	return (0);
}

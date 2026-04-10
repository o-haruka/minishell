#include "libft.h"
#include "minishell.h"
#include <readline/history.h>
#include <readline/readline.h>

static char	**g_envp;

/*
** ループ処理を行う関数
** readlineはmallocされた文字列を返すので、使い終わったらfreeが必要です。
** lineがNULLの場合は、Ctrl-D (EOF) が入力されたことを意味します。
*/
void    minishell_loop(t_shell *shell)
{
	char *line;

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
		// [Debug] トークンの中身を見てみる（必要なときだけ呼び出し）
		// debug_print_tokens(shell->tokens);


		//TODO  3. Parser : トークンを解析して shell->cmds に変換（今後実装）
		shell->cmds = parse(shell->tokens);
		// [Debug] パース結果（t_cmdリスト）を見たいときだけ有効化
		// debug_print_cmds(shell->cmds);

		// $ 展開 (Expander)
		ft_expand_args(shell);
		if (shell->cmds && shell->cmds->args && shell->cmds->args[0])
			execute_command(shell->cmds->args, g_envp);

		//TODO 3. 実行 (Executor) コマンドを実行し、終了ステータスを更新（今後実装）
		// shell->last_status = execute(shell);

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

	// 1. 初期化 (構造体の中身を0クリア) libftのmemsetでもOK
	// shell = (t_shell){0};
	shell = (t_shell){NULL, NULL, NULL, 0}; //わかりやすい
	// shell = (t_shell){.env = NULL, .tokens = NULL, .cmds = NULL, .last_status = 0}; //可読性は上がるが長い
	g_envp = envp;

	//! 環境変数の取得。envpを解析して、扱いやすい連結リスト(t_env)に変換する
	// TODO: (展開 (Expander)」の実装に取り掛かる直前)に実装
	shell.env = init_env(envp);

	//2. シグナル初期化
	// setup_signals();

	//3. メインループの開始
	minishell_loop(&shell);

	//4. 終了前の全体メモリ解放
	// free_all_data(env_list);
	return (0);
}

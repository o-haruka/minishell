#include "minishell.h"

/*
** ループ処理を行う関数
** readlineはmallocされた文字列を返すので、使い終わったらfreeが必要です。
** lineがNULLの場合は、Ctrl-D (EOF) が入力されたことを意味します。
*/
void	minishell_loop(t_shell *shell)
{
	char	*line;

	while (1)
	{
		g_signal = 0; // シグナルステータスをリセット
		// ★★ 1. readlineで待機する直前にエコーをオフにする
        disable_echoctl();
		if (isatty(STDIN_FILENO))
			// isatty(fd) は「そのファイルディスクリプタがターミナル（人間が操作している端末）に繋がっているか？」を確認する関数
			line = readline(GREEN "minishell> " RESET); // 対話モード: プロンプトあり
		else
			line = get_next_line(STDIN_FILENO); // パイプ入力時: プロンプトなし

		// ★★ 2. 入力が終わったら即座にエコー設定を元に戻す
        // これにより、この後の ft_execute で実行される外部コマンドでは ^C が表示されるようになります
        restore_echoctl();
		// TODO free必須(readlineはmallocする)
		if (line == NULL)
		{
			if (isatty(STDIN_FILENO))
				// TODO 本家ではここに(break前に)printf("exit\n");が入るらしい
				ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		// get_next_lineは末尾に\nが付くので除去
		if (!isatty(STDIN_FILENO))
			line[ft_strlen(line) - 1] = '\0'; // \n → \0
		// シグナルが発生した場合は継続
		if (g_signal == SIGINT)
		{
			// Ctrl-Cが押されていたら、$? を 130 (128 + 2) に更新する
			//
			shell->last_status = 128 + g_signal;
			// free(line);
			// continue ;
		}
		//★-----------------------------------★
		// 1. 読み込み
		//★-----------------------------------★
		if (*line)
			add_history(line);
		//★-----------------------------------★
		// 2. 字句解析 (Lexer)
		//★-----------------------------------★
		shell->tokens = tokenize(line);
		if (shell->tokens == NULL)
		{
			free(line);
			// TODO 必要に応じてエラーメッセージを表示
			// ft_putendl_fd("lexer error", 2);
			continue ;
		}
		// [Debug] トークンの中身を見てみる（必要なときだけ呼び出し）
		// debug_print_tokens(shell->tokens);
		//★-----------------------------------★
		// 3. Parser : トークンを解析して shell->cmds に変換（今後実装）
		//★-----------------------------------★
		shell->cmds = parse(shell->tokens);
		if (shell->cmds == NULL)
		{
			token_free(&(shell->tokens));
			free(line);
			// TODO 必要ならエラーメッセージ
			// ft_putendl_fd("parse error", 2);
			continue ;
		}
		// [Debug] パース結果（t_cmdリスト）を見たいときだけ有効化
		// debug_print_cmds(shell->cmds);
		//★-----------------------------------★
		// 4. $ 展開 (Expander)
		//★-----------------------------------★
		ft_expand_args(shell);
		//★-----------------------------------★
		// 5. 実行 (Executor) コマンドを実行
		//★-----------------------------------★
		if (shell->cmds)
			ft_execute(shell);
		//★-----------------------------------★
		// 6. 後始末
		//★-----------------------------------★
		free_cmds_list(shell->cmds);  //コマンドリスト全体を解放
		token_free(&(shell->tokens)); // トークンリスト全体を解放
		free(line);                   // readlienで確保したメモリを解放
	}
	free_env(shell->env);
	rl_clear_history(); // readlineの履歴をクリア
}

int	main(int argc, char **argv, char **envp)
{
	t_shell shell; //!全体の状態を持つ構造体
	// t_env *env_list; //! 逆に環境変数用の変数が必要。 環境変数の連結リストの先頭ポインタ
	(void)argc;
	(void)argv;
	// 1. 初期化 (構造体の中身を0クリア) libftのmemsetでもOK
	// 1. t_shell を初期化する（フィールド順: env, cmds, tokens, last_status, envp）
	// ※ {.env=NULL, .cmds=NULL, ...} と書くと読みやすいが行が長くなるため省略形を使用
	// envp を構造体で保持
	shell = (t_shell){NULL, NULL, NULL, 0};
	//! 環境変数の取得。envpを解析して、扱いやすい連結リスト(t_env)に変換する
	shell.env = init_env(envp);
	if (!shell.env)
		return (1);
	// 2. シグナル初期化
	setup_signals();
	// 3. メインループの開始
	minishell_loop(&shell);
	// 4. 終了前の全体メモリ解放
	// free_all_data(env_list);
	return (shell.last_status);
}

#include "minishell.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>

int		g_signal = 0;

/*
** 入力待ち時のSIGINT（Ctrl-C）用ハンドラ
** ユーザーがプロンプトで入力中にCtrl-Cを押した場合、
** 入力をキャンセルして新しいプロンプトを表示する。
*/
void	sigint_handler(int sig)
{
	g_signal = sig;
	// 1. ユーザーが見ている画面上で改行する
	write(STDOUT_FILENO, "\n", 1);
	// 2. readlineが現在保持している入力バッファを「空」に置き換える
	rl_replace_line("", 0);
	// 3. readlineに「カーソルは新しい行の先頭にある」と伝える
	rl_on_new_line();
	// 4. プロンプトとバッファ（今は空）を再描画する
	//    今、自分が記憶している『プロンプト』と『バッファ』をくっつけて、画面を最新状態に描き直す（再描画する）
	rl_redisplay();
	// ※終了ステータスは呼び出し元で設定することが多い
}

void	setup_signals(void)
/*
** 入力待ち時のシグナル設定（メインループ用）
**  - SIGINT: 独自ハンドラ（プロンプトをリセット）
**  - SIGQUIT: 無視
** readlineを使ったプロンプト入力時に呼び出す。
*/
void setup_signals(void)
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask); // シグナルマスクを空にする
	sa.sa_flags = 0;          // SA_SIGINFOフラグを削除

	// SIGINT（Ctrl-C）: 入力キャンセル用ハンドラ
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	// SIGQUIT（Ctrl-\）: 無視
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}
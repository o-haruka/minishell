#include "minishell.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

int g_signal = 0;

/*
** Ctrl-C (SIGINT) が押された時のハンドラ
*/
void sigint_handler(int sig)
{
    g_signal = sig;

    // 1. ユーザーが見ている画面上で改行する
    write(STDOUT_FILENO, "\n", 1);

    // 2. readlineが現在保持している入力バッファを「空」に置き換える
    rl_replace_line("", 0);

    // 3. readlineに「カーソルは新しい行の先頭にある」と伝える
    rl_on_new_line();

    // 4. プロンプトとバッファ（今は空）を再描画する
    rl_redisplay();

    // 終了ステータス用
    // g_signal = 130; // 128 + SIGINT(2) = 130
}

void setup_signals(void){
    struct sigaction    sa;
    
    sigemptyset(&sa.sa_mask); // シグナルマスクを空にする
    sa.sa_flags = 0; // SA_SIGINFOフラグを削除

    // SIGINTのハンドラを設定
    sa.sa_handler = sigint_handler; // ハンドラ関数を設定
    sigaction(SIGINT, &sa, NULL); // SIGINTに対して設定を適用

    // SIGQUITは無視する
    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
}
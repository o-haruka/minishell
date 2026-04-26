#include "minishell.h"

/* ^C などのコントロール文字のエコーを非表示にする */
void disable_echoctl(void)
{
    struct termios term;

    // 現在の端末設定を取得
    if (tcgetattr(STDIN_FILENO, &term) == -1)
        return ;
    // ECHOCTLフラグをオフにする
    term.c_lflag &= ~(ECHOCTL);
    // 変更を即座に適用
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

/* ^C などのエコーを元の表示される状態に戻す */
void restore_echoctl(void)
{
    struct termios term;

    // 現在の端末設定を取得
    if (tcgetattr(STDIN_FILENO, &term) == -1)
        return ;
    // ECHOCTLフラグをオンに戻す
    term.c_lflag |= ECHOCTL;
    // 変更を即座に適用
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
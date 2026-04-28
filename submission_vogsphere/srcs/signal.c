/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:35:24 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:45:55 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		g_signal = 0;

/*
** 入力待ち時のSIGINT（Ctrl-C）用ハンドラ
** ユーザーがプロンプトで入力中にCtrl-Cを押した場合、
** 入力をキャンセルして新しいプロンプトを表示する。
*/
void	sigint_handler(int sig)
{
	g_signal = sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

/*
** コマンド実行中（waitpid中）の親プロセス用のシグナル設定
** SIGINT（Ctrl-C）  -> 無視 (SIG_IGN)（子プロセスにのみ伝播）
** SIGQUIT（Ctrl-\） -> 無視 (SIG_IGN)
** これにより、親シェルがコマンド実行中に割り込まれないようにする。
*/
void	set_signal_for_parent_wait(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/*
** execveで実行される子プロセス用のシグナル設定
** SIGINT（Ctrl-C）  -> デフォルトの挙動に戻す (SIG_DFL)
** SIGQUIT（Ctrl-\） -> デフォルトの挙動に戻す (SIG_DFL)
** これにより、lsやcatなどの外部コマンドが通常通りシグナルで終了できる。
*/
void	set_signal_for_child(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/*
** 入力待ち時のシグナル設定（メインループ用）
**  - SIGINT: 独自ハンドラ（プロンプトをリセット）
**  - SIGQUIT: 無視
** readlineを使ったプロンプト入力時に呼び出す。
*/
void	setup_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

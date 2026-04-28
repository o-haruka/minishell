/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:50 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:44:44 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/wait.h>

/*
** 【heredoc とは】
** 通常の入力リダイレクト（< file）はファイルから読み込むが、
** heredoc（<< WORD）はターミナルで直接入力した内容を stdin に流す。
**
** イメージ：
**   cat << EOF    ← "EOF が来るまで入力を受け付けるよ" という宣言。EOF以外の文字列でもよい
**   > hello       ← ユーザーが入力（これが stdin に流れる）
**   > world       ← ユーザーが入力（これが stdin に流れる）
**   > EOF         ← デリミタ（「ここで入力終わり」を伝えるための目印となる文字列）が来たので終了
**   hello         ← cat がそれを受け取って出力
**   world
**
** 【この関数の役割】
** heredoc の入力受付ループ。
** "> " プロンプトを出しながら1行ずつ読み、
** パイプの書き込み端（write_fd）に流し続ける。
** 以下のどちらかが来たら終了する：
**   - delimiter と完全一致する行（正常終了）
**   - Ctrl+D による EOF（readline が NULL を返す）
**		Ctrl+D は「入力の終わり（EOF）を伝えるキー操作」
*/

// 1. 一番末端の下請け（誰からも呼ばれる基礎）
static void	read_heredoc_input(int write_fd, char *delimiter)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
			&& ft_strlen(line) == ft_strlen(delimiter))
		{
			free(line);
			break ;
		}
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
}

// 2. 子プロセス用の下請け（中で read_heredoc_input を呼ぶ）
static void	exec_heredoc_child(int *pipefd, char *delimiter)
{
	set_signal_for_child();
	close(pipefd[0]);
	read_heredoc_input(pipefd[1], delimiter);
	close(pipefd[1]);
	exit(0);
}

// 3. 親プロセス用の下請け
static int	wait_heredoc_parent(pid_t pid, int *pipefd)
{
	int	status;

	close(pipefd[1]);
	set_signal_for_parent_wait();
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		write(STDERR_FILENO, "\n", 1);
		close(pipefd[0]);
		return (-1);
	}
	return (pipefd[0]);
}

/*
** heredoc を処理する。
** pipe() でパイプを作り、入力を書き込んだ後
** 読み込み端を stdin(0番) に dup2 で付け替える。
** 成功: 0, 失敗: -1 を返す。
*/
int	apply_heredoc(char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		exec_heredoc_child(pipefd, delimiter);
	return (wait_heredoc_parent(pid, pipefd));
}

/*
** fork前に全コマンドの heredoc を処理して
** t_redir->fd に読み込み端の fd を保存する。
** 成功: 0, 失敗: -1
*/
int	prepare_heredocs(t_cmd *cmd)
{
	t_redir	*redir;

	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->kind == TK_HEREDOC)
			{
				redir->fd = apply_heredoc(redir->file);
				if (redir->fd == -1)
					return (-1);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0);
}

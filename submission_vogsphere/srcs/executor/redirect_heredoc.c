/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:32:50 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/29 00:00:00 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/wait.h>

/*
** 子プロセスの処理。
** シグナルをデフォルトに設定してから "> " プロンプトで1行ずつ読み込み、
** delimiter と一致するか Ctrl+D が来るまでパイプに書き続ける。
*/
static void	exec_heredoc_child(int *pipefd, char *delimiter)
{
	char	*line;

	set_signal_for_child();
	close(pipefd[0]);
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
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	exit(0);
}

/*
** heredoc を処理する。
** pipe() でパイプを作り、子プロセスで入力を書き込んだ後
** 親プロセスが子の終了を待ち、読み込み端の fd を返す。
** 成功: pipefd[0] / 失敗: -1
*/
int	apply_heredoc(char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;

	if (pipe(pipefd) == -1)
	{
		print_error_msg(NULL, "pipe", strerror(errno));
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		print_error_msg(NULL, "fork", strerror(errno));
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		exec_heredoc_child(pipefd, delimiter);
	close(pipefd[1]);
	if (wait_for_child(pid, &status) == -1)
		return (close(pipefd[0]), -1);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		return (write(STDERR_FILENO, "\n", 1), close(pipefd[0]), -1);
	return (pipefd[0]);
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

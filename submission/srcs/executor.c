#include "libft.h"
#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char	*get_executable_path(char *cmd, char **envp)
{
	// コマンド自体に '/' が含まれている場合はそのまま使う
	// (例: /bin/ls, ./minishell)
	if (strchr(cmd, '/'))
	{
		return (ft_strdup(cmd));
	}
	// それ以外はPATH環境変数から探す
	return (search_path(cmd, envp));
}

void	execute_command(char **cmdline, char **envp)
{
	// ここにコマンド実行のロジックを実装します
	// 例: fork()、execve()、wait() などを使用してコマンドを実行
	(void)cmdline; // プレースホルダ: 実際の実装ではcmdlineを使用します
	(void)envp;    // プレースホルダ: 実際の実装ではenvpを使用します

	int status;
	pid_t pid;
	char *cmd_path;
	struct sigaction sa_default, sa_ignore, sa_old_int, sa_old_quit;

	cmd_path = get_executable_path(cmdline[0], envp);
	if (cmd_path == NULL)
	{
		//見つからなかった場合のエラー表示
		fprintf(stderr, "Command not found: %s\n", cmdline[0]);
		fprintf(stderr, cmdline[0], ft_strlen(cmdline[0]));
		fprintf(stderr, "\n");
		return ; // 子プロセスを作らずに戻る
	}

	// 子プロセス実行中は親プロセスのシグナル処理を変更
	sa_default.sa_handler = SIG_DFL;
	sa_ignore.sa_handler = SIG_IGN;
	sigemptyset(&sa_default.sa_mask);
	sigemptyset(&sa_ignore.sa_mask);
	sa_default.sa_flags = 0;
	sa_ignore.sa_flags = 0;

	sigaction(SIGINT, &sa_ignore, &sa_old_int);   // 親はSIGINTを無視
	sigaction(SIGQUIT, &sa_ignore, &sa_old_quit); // 親はSIGQUITを無視

	pid = fork();

	if (pid < 0)
	{
		perror("fork");
		// シグナル処理を元に戻す
		sigaction(SIGINT, &sa_old_int, NULL);
		sigaction(SIGQUIT, &sa_old_quit, NULL);
		free(cmd_path);
		return ;
	}

	if (pid == 0)
	{
		// 子プロセス: デフォルトのシグナル処理に戻す
		sigaction(SIGINT, &sa_default, NULL);
		sigaction(SIGQUIT, &sa_default, NULL);

		if (execve(cmd_path, cmdline, envp) == -1)
		{
			perror("execve");
			exit(127); // コマンドが見つからない場合の終了コード
		}
	}
	else
	{
		waitpid(pid, &status, 0);

		// 子プロセスがシグナルで終了した場合は改行を出力
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			write(STDOUT_FILENO, "\n", 1);
		}

		// 親プロセス: 元のシグナル処理に戻す
		sigaction(SIGINT, &sa_old_int, NULL);
		sigaction(SIGQUIT, &sa_old_quit, NULL);

		free(cmd_path); // 確保したメモリを解放
	}
}
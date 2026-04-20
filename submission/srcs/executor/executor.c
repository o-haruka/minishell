/*homura担当*/

#include "libft.h"
#include "minishell.h"
#include <sys/wait.h>

/*
** 4. 子プロセスの処理 (pid == 0)
*/
static void	exec_child(char *path, t_cmd *cmd, char **envp)
{
	// execve実行の前に fd を付け替える。
	// execve で起動するコマンドは、ファイルを読み書きするために fd 0番・1番だけを使う。それ以外は見ない。
	// execve 後は fd を変更できないため、先に付け替えておく必要がある
	if (ft_apply_redirs(cmd) == -1)
	{
		free(path);
		exit(1);
	}
	// execveを使って、子プロセスを別のプログラムに生まれ変わらせる
	if (execve(path, cmd->args, envp) == -1)
	{
		// execveが失敗した場合（通常はここに来ないはずだが、念のため）
		perror("minishell: execve");
		free(path);
		exit(1); // 子プロセスを終了させる
	}
}

/*
** 5. 親プロセスの処理 (pid > 0)
*/
static void	exec_parent(pid_t pid, t_shell *shell, char *path)
{
	int	status;

	// 親プロセスは子プロセスが終了するのを待つ
	if (waitpid(pid, &status, 0) == -1){
		perror("minishell: waitpid");
		free(path);
		return;
	}

	// 子プロセスの終了ステータスを last_status に保存する
	// WIFEXITEDマクロで正常終了か確認し、WEXITSTATUSマクロでステータス値を取り出す
	if (WIFEXITED(status))
		shell->last_status = WEXITSTATUS(status);

	free(path); // 親プロセスで、もう使わないパスを解放
}

/*
** 単一コマンドを実行する関数
*/
void    ft_execute(t_shell *shell)
{
	pid_t	pid;
	char	*path;
	t_cmd	*cmd;

	cmd = shell->cmds;
	// 1. 実行するコマンド名が空でないか確認
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;

	// ★★★ パイプ実装に伴い追加 ★★★
    // cmd->next != NULL = パイプで繋がったコマンドが存在する
    // 例: ls | grep → cmd[ls]->next = cmd[grep] → NULL ではない
    // 例: ls         → cmd[ls]->next = NULL      → ここには入らない
    if (cmd->next != NULL)
    {
        ft_execute_pipeline(shell); // パイプライン専用関数に丸投げ
        return ;                    // 以降の単一コマンド処理には進まない
    }
	// ★ 1. もしビルトインコマンドだったら？
    if (is_builtin(cmd->args[0]))
    {
        // execve は絶対に呼ばない！  ただのC言語の関数（ft_cdやft_pwdなど）を呼び出すだけ。
        shell->last_status = exec_builtin(cmd, shell);
        return ; // ★重要: 関数が終わったら、下の fork や execve に進まずにそのまま帰る！
    }

	// ★ 2. ビルトインじゃなかったら？（外部コマンド）
	// 2. 実行ファイルの絶対パスを探す
	path = search_path(cmd->args[0], shell->env);
	if (!path)
	{
		// パスが見つからなかった場合のエラー処理
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": command not found", STDERR_FILENO);
		shell->last_status = 127; // コマンドが見つからない時の標準的な終了ステータス
		return ;
	}

	// 3. プロセスを分岐させる (fork)
	pid = fork();
	if (pid < 0)
	{
		// forkに失敗した場合
		perror("minishell: fork");
		free(path);
		return ;
	}

	if (pid == 0) //4. 子プロセスの処理 (pid == 0)
		exec_child(path, cmd, shell->envp);
	else  //5. 親プロセスの処理 (pid > 0)
		exec_parent(pid, shell, path);
}


//シグナルで参考にするかもしれないので残しておく。

// void    ft_execute(t_shell *shell);
// {
// 	int status;
// 	pid_t pid;
// 	char *cmd_path;
// 	struct sigaction sa_default, sa_ignore, sa_old_int, sa_old_quit;

// 	cmd_path = get_executable_path(cmdline[0], envp);
// 	if (cmd_path == NULL)
// 	{
// 		//見つからなかった場合のエラー表示
// 		fprintf(stderr, "Command not found: %s\n", cmdline[0]);
// 		fprintf(stderr, cmdline[0], ft_strlen(cmdline[0]));
// 		fprintf(stderr, "\n");
// 		return ; // 子プロセスを作らずに戻る
// 	}

// 	// 子プロセス実行中は親プロセスのシグナル処理を変更
// 	sa_default.sa_handler = SIG_DFL;
// 	sa_ignore.sa_handler = SIG_IGN;
// 	sigemptyset(&sa_default.sa_mask);
// 	sigemptyset(&sa_ignore.sa_mask);
// 	sa_default.sa_flags = 0;
// 	sa_ignore.sa_flags = 0;

// 	sigaction(SIGINT, &sa_ignore, &sa_old_int);   // 親はSIGINTを無視
// 	sigaction(SIGQUIT, &sa_ignore, &sa_old_quit); // 親はSIGQUITを無視

// 	pid = fork();

// 	if (pid < 0)
// 	{
// 		perror("fork");
// 		// シグナル処理を元に戻す
// 		sigaction(SIGINT, &sa_old_int, NULL);
// 		sigaction(SIGQUIT, &sa_old_quit, NULL);
// 		free(cmd_path);
// 		return ;
// 	}

// 	if (pid == 0)
// 	{
// 		// 子プロセス: デフォルトのシグナル処理に戻す
// 		sigaction(SIGINT, &sa_default, NULL);
// 		sigaction(SIGQUIT, &sa_default, NULL);

// 		if (execve(cmd_path, cmdline, envp) == -1)
// 		{
// 			perror("execve");
// 			exit(127); // コマンドが見つからない場合の終了コード
// 		}
// 	}
// 	else
// 	{
// 		waitpid(pid, &status, 0);

// 		// 子プロセスがシグナルで終了した場合は改行を出力
// 		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
// 		{
// 			write(STDOUT_FILENO, "\n", 1);
// 		}

// 		// 親プロセス: 元のシグナル処理に戻す
// 		sigaction(SIGINT, &sa_old_int, NULL);
// 		sigaction(SIGQUIT, &sa_old_quit, NULL);

// 		free(cmd_path); // 確保したメモリを解放
// 	}
// }

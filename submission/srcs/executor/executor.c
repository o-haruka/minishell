/*homura担当*/

#include "libft.h"
#include "minishell.h"
#include <sys/wait.h>

/*
** 4. 子プロセスの処理 (pid == 0)
*/
static void	exec_child(char *path, t_cmd *cmd, t_shell *shell)
{
	char **current_envp;

	// execve実行の前に fd を付け替える。
	// execve で起動するコマンドは、ファイルを読み書きするために fd 0番・1番だけを使う。それ以外は見ない。
	// execve 後は fd を変更できないため、先に付け替えておく必要がある
	if (ft_apply_redirs(cmd) == -1)
	{
		free(path);
		exit(1);
	}

	// 最新の環境変数リストから配列を生成する
	current_envp = env_to_envp(shell->env);

	// execveを使って、子プロセスを別のプログラムに生まれ変わらせる
	if (execve(path, cmd->args, current_envp) == -1)
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
	if (WIFEXITED(status)) // ★普通に終了した場合 (exitなど)
		shell->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status)) // ★子プロセスがシグナルで強制終了した場合
	{
		shell->last_status = 128 + WTERMSIG(status); // 例: 128 + 2 = 130
		
		// bashの仕様に合わせるための見た目の処理
		if (WTERMSIG(status) == SIGQUIT)
			ft_putendl_fd("Quit: 3", STDERR_FILENO); // Ctrl-\ で殺されたら Quit: 3 を出す
		else if (WTERMSIG(status) == SIGINT)
			write(STDERR_FILENO, "\n", 1);           // ★ これが ^C の後に改行を入れてくれる！
	}

	// ★ 追加：待機が終わったので、親プロセスを「無視」から「元のハンドラ設定」に戻す
	setup_signals();

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

	if (pid == 0){ //4. 子プロセスの処理 (pid == 0)
		set_signal_for_child(); //OSのデフォルト（強制終了）
		exec_child(path, cmd, shell);
	}
	else{  //5. 親プロセスの処理 (pid > 0)
		set_signal_for_parent_wait(); //シグナル飛んできても無視
		exec_parent(pid, shell, path);
	}
}

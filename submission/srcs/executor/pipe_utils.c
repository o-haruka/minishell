#include "minishell.h"


/*
** パイプで繋がったコマンドの個数を数えて返す。
** 例) [ls] -> [grep] -> [wc] -> NULL なら 3 を返す。
*/
int	count_cmds(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}


/*
** pipe_count 本のパイプを全て作る。
** 1本でも失敗したら -1 を返す。
*/
int	open_all_pipes(int (*pipes)[2], int pipe_count)
{
	int	i;

	i = 0;
	while (i < pipe_count)
	{
		if (pipe(pipes[i]) == -1)
		{
			perror("minishell: pipe");
			return (-1);
		}
		i++;
	}
	return (0);
}


/*
** 全パイプのfdを閉じる。
** 子プロセス内で dup2 後に呼ぶ。
** 親プロセスでも fork 後に呼ぶ。
*/
void	close_all_pipes(int (*pipes)[2], int pipe_count)
{
	int	i;

	i = 0;
	while (i < pipe_count)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

/*
** 全子プロセスの終了を待ち、最後のコマンドの
** 終了ステータスを shell->last_status に保存する。
*/
void	wait_all_cmds(pid_t *pids, int cmd_count, t_shell *shell)
{
	int	i;
	int	status;

	i = 0;
	status = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	// bash の仕様: パイプラインの $? は最後のコマンドの終了コード
	if (WIFEXITED(status))
		shell->last_status = WEXITSTATUS(status);
}

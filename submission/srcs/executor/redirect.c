/*hkuninag担当*/

/*
redirect.c の役割： 「minishell.h の redirs リストを走査して、dup2 で fd を付け替えること」
*/

/*
dup2 が必要な理由：
execve で起動するプログラムが「0番・1番・2番」しか見ないから。
こちら側でどんな fd を用意しても、起動後のプログラムには届かない。
だから「プログラムが必ず見る番号」に付け替えてから渡す必要がある。
*/

#include "minishell.h"
#include "libft.h"
#include <fcntl.h>


/*
** redir->kind に対応する fd を open して返す。
** target_fd（0 or 1）もポインタ経由で設定する。
** 失敗時は -1 を返す。
*/
static int	open_redir_fd(t_redir *redir, int *target_fd)
{
    if (redir->kind == TK_REDIRECT_IN)
    {
        *target_fd = STDIN_FILENO; // 0番(stdin)を付け替え対象にする
        return (open(redir->file, O_RDONLY)); // 読み込み専用で開く
    }
    if (redir->kind == TK_REDIRECT_OUT)
    {
        *target_fd = STDOUT_FILENO;  // 1番(stdout)を付け替え対象にする
        return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644)); // 上書きで開く（なければ作る）
    }
    if (redir->kind == TK_APPEND)
    {
        *target_fd = STDOUT_FILENO; // 1番(stdout)を付け替え対象にする
        return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644)); // 追記で開く（なければ作る）
    }
    return (-2); // TK_HEREDOC など、ここでは扱わない種類
}

/*
** リダイレクト1件分を処理する。
** kind に応じて open し、dup2 で stdin/stdout を付け替える。
** 成功: 0, 失敗: -1 を返す。
*/
static int	apply_one_redir(t_redir *redir)
{
	int	fd;
	int	target_fd;

	target_fd = -1;
	fd = open_redir_fd(redir, &target_fd); // ファイルを開き、fd番号を受け取る
	if (fd == -2) // 対象外の kind（HEREDOCなど）はスキップ
		return (0);
	if (fd == -1)
	{
		perror(redir->file); // open 失敗 → ファイル名付きエラー出力
		return (-1);
	}
	if (dup2(fd, target_fd) == -1) // fd の接続先を 0番 or 1番 にコピーする
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd); // dup2 済みなので元の fd は不要、閉じる
	return (0);
}

/*
** cmd->redirs リストを順番に処理する。
** 1件でも失敗したら -1 を返す。
** executor.c の子プロセス内から呼ばれる。
*/
int	ft_apply_redirs(t_cmd *cmd)
{
    t_redir *r;

    if (!cmd)
        return (0);
    r = cmd->redirs; // リダイレクトリストの先頭から開始
    while (r)
    {
        if (apply_one_redir(r) == -1) // 1件失敗したらそこで中断
            return (-1);
        r = r->next;  // 次のリダイレクトへ
    }
    return (0);
}

/*hkuninag担当*/

/*
**　redirect.c の役割： 「minishell.h の redirs リストを走査して、dup2 で fd を付け替えること」
**　その後、付け替え済みの fd をexecve()が引き継いでコマンド起動
**
** 【全体の流れにおける位置づけ】
**
**   入力文字列
**     └─ tokenize()       字句解析  → トークンリスト
**     └─ parse()          構文解析  → cmd->redirs に種類とファイル名が入る
**     └─ ft_expand_args() 変数展開  → $VAR などを展開
**     └─ ft_execute()     実行エンジン
**          └─ fork()
**               └─ 子プロセス:
**                    └─ ft_apply_redirs()  ← ここがこのファイルの仕事
**                         open()  : ファイルを開いて fd を得る
**                         dup2()  : 0番(stdin) or 1番(stdout) を付け替える
**                         close() : 元の fd を閉じる
**                    └─ execve()  付け替え済みの fd を引き継いでコマンド起動
*/

/*
dup2 が必要な理由：
execve で起動するプログラムが「0番・1番・2番」しか見ないから。
こちら側でどんな fd を用意しても、起動後のプログラムには届かない。
だから「プログラムが必ず見る番号」に付け替えてから渡す必要がある。
*/

#include "minishell.h"
#include <fcntl.h>

#define REDIR_OPEN_ERROR (-1)
#define REDIR_OPEN_SKIP  (-2)

/*
** redir->kind に対応する fd を open して返す。
** target_fd（0 or 1）もポインタ経由で設定する。
** 失敗時は -1 を返す。
*/
static int	open_redir_fd(t_redir *redir, int *target_fd)
{
    if (redir->kind == TK_REDIRECT_IN) // '<' のとき : ファイルをキーボード入力の代わりに読む
    {
        *target_fd = STDIN_FILENO; // 0番(stdin)を付け替え対象にする
        return (open(redir->file, O_RDONLY)); // 読み込み専用で開く
    }
    if (redir->kind == TK_REDIRECT_OUT) // '>' のとき : 画面に出す代わりにファイルへ上書きする
    {
        *target_fd = STDOUT_FILENO;  // 1番(stdout)を付け替え対象にする
        return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644)); // 書き込み用で開く: なければ作成、あれば中身を空にして先頭から書く（上書き）
			// 0644: 省略不可。O_CREATE をつけた時に必須。6=所有者rw(読み書き可), 4=グループr(読み込みのみ), 4=他人r(読み込みのみ)
    }
    if (redir->kind == TK_APPEND) // '>>' のとき : 画面に出す代わりにファイルの末尾へ書き足す
    {
        *target_fd = STDOUT_FILENO; // 1番(stdout)を付け替え対象にする
        return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644)); // 追記で開く（なければ作る）
    }
    return (REDIR_OPEN_SKIP); // 対象外の kind
}

/* heredoc のリダイレクト処理をまとめた関数 */
static int  apply_heredoc_redir(t_redir *redir)
{
    if (redir->fd != -1)
    {
        if (dup2(redir->fd, STDIN_FILENO) == -1)
            return (-1);
        close(redir->fd);
        return (0);
    }
    redir->fd = apply_heredoc(redir->file);
    if (redir->fd == -1)
        return (-1);
    if (dup2(redir->fd, STDIN_FILENO) == -1)
        return (-1);
    close(redir->fd);
    return (0);
}

static int	apply_one_redir(t_redir *redir)
{
	int	fd;
	int	target_fd;

	if (redir->kind == TK_HEREDOC)
    	return (apply_heredoc_redir(redir));
	target_fd = -1; // open_redir_fd 内で 0 or 1 に上書きされる
	fd = open_redir_fd(redir, &target_fd); // ファイルを開き、fd番号を受け取る。target_fd も 0 or 1 に上書きされる
    if (fd == REDIR_OPEN_SKIP) // 対象外の kind はスキップ
		return (0);
    if (fd == REDIR_OPEN_ERROR)
	{
        print_error_msg(NULL, redir->file, strerror(errno));
		return (-1);
	}
	if (dup2(fd, target_fd) == -1) // fd の接続先を target_fd (0番 or 1番) にコピーする
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
    t_redir	*r;

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

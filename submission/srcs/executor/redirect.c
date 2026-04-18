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
#include "libft.h"
#include <fcntl.h>
#include <readline/readline.h>

static int	apply_heredoc(char *delimiter);

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
    return (-2); // 対象外の kind
}
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
static void	read_heredoc_input(int write_fd, char *delimiter)
{
    char    *line;

    while (1)
    {
        line = readline("> "); // "> " をプロンプトとして表示し、ユーザーの1行入力を待つ
        if (!line)                  // Ctrl-Dが押されたら、readline が NULLを返すため、終了
            break ;
        if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
            && ft_strlen(line) == ft_strlen(delimiter))
									// line がデリミタと完全一致するかチェック
									// ft_strncmp: デリミタの長さ分だけ比較（前の部分が一致）
									// ft_strlen(line) == ft_strlen(delimiter): 長さが一致（余分な文字がない）
        {
            free(line); 			// readline() で動的割り当てされたメモリを解放
            break ;                 // デリミタ行が来たら終了
        }
        write(write_fd, line, ft_strlen(line));
        write(write_fd, "\n", 1); // 改行も書き込む
        free(line);
    }
}

/*
** heredoc を処理する。
** pipe() でパイプを作り、入力を書き込んだ後
** 読み込み端を stdin(0番) に dup2 で付け替える。
** 成功: 0, 失敗: -1 を返す。
*/
static int	apply_heredoc(char *delimiter)
{
	int	pipefd[2]; // pipefd[0]=読み込み端, pipefd[1]=書き込み端

	if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (-1);
    }
	read_heredoc_input(pipefd[1], delimiter); // デリミタまでの入力を書き込む
    close(pipefd[1]);                         // 書き終わり、書き込み端を閉じる
    if (dup2(pipefd[0], STDIN_FILENO) == -1)  // 読み込み端を stdin に付け替える
    {
        perror("dup2");
        close(pipefd[0]);
        return (-1);
    }
    close(pipefd[0]);                         // dup2 済みなので閉じる
    return (0);
}

/*
** リダイレクト (redirs リスト) 1件分を処理する。
** HEREDOC は apply_heredoc() で処理し、
** それ以外は open → dup2 で stdin/stdout を付け替え → close の流れで処理する。
** 成功: 0, 失敗: -1 を返す。
*/
static int	apply_one_redir(t_redir *redir)
{
	int	fd;
	int	target_fd;

	if (redir->kind == TK_HEREDOC)
    	return (apply_heredoc(redir->file)); // file にデリミタが入っている
	target_fd = -1; // open_redir_fd 内で 0 or 1 に上書きされる
	fd = open_redir_fd(redir, &target_fd); // ファイルを開き、fd番号を受け取る。target_fd も 0 or 1 に上書きされる
	if (fd == -2) // 対象外の kind はスキップ
		return (0);
	if (fd == -1)
	{
		perror(redir->file); // open 失敗 → ファイル名付きエラー出力
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

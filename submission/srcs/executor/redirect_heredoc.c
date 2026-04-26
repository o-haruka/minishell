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

// 2. 子プロセス用の下請け（中で read_heredoc_input を呼ぶ）
static void exec_heredoc_child(int *pipefd, char *delimiter)
{
    // 1. ここは Heredoc 専用の空間。Ctrl-C で死ぬようにデフォルトに戻す！
	set_signal_for_child();
	
	// 2. 子プロセスはパイプの出口(0)は使わないので閉じる
	close(pipefd[0]);

	// 3. 実際の読み込み作業を子プロセスにやらせる
	read_heredoc_input(pipefd[1], delimiter); // デリミタまでの入力を書き込む
	
	// 4. 書き終わったら出口を閉じて、正常に(0で)生涯を終える
	close(pipefd[1]); // 書き終わり、書き込み端を閉じる
	exit(0);
}

// 3. 親プロセス用の下請け
static int  wait_heredoc_parent(pid_t pid, int *pipefd)
{
	int status;

	close(pipefd[1]); // 1. 親はパイプの入口(1)は使わないので閉じる
	set_signal_for_parent_wait(); // 2. 待機中は暴発しないようにシグナルを無視にする！
	waitpid(pid, &status, 0); // 3. 子プロセス（Heredoc読み込み）が終わるのを待つ
	setup_signals(); // 4. 魔法のハンドラを復活させる
	// ★Ctrl-C でキャンセルされたかのチェック
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		// 子プロセスが Ctrl-C で死んだ場合！
		
		write(STDERR_FILENO, "\n", 1); // 親プロセスはここで改行を出力し、
		close(pipefd[0]); // パイプも不要になったので閉じて、
		// 変更前： return (-1);
		// 変更後： エラーを返すのではなく、自分（第2層）も 130 で死ぬ！
		// これにより、親（第1層）に確実に 130 が伝わる。
		exit(130);
	}
	// 5. 無事に終わった場合（Ctrl-Cされてない場合）、パイプを stdin に繋ぐ
	if (dup2(pipefd[0], STDIN_FILENO) == -1) // 読み込み端を stdin に付け替え
	{
		perror("dup2");
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	return (0);
}

/*
** heredoc を処理する。
** pipe() でパイプを作り、入力を書き込んだ後
** 読み込み端を stdin(0番) に dup2 で付け替える。
** 成功: 0, 失敗: -1 を返す。
*/
int	apply_heredoc(char *delimiter)
{
	int	pipefd[2]; // pipefd[0]=読み込み端, pipefd[1]=書き込み端
    pid_t	pid;

	if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (-1);
    }

    // ★ 追加1：Heredoc読み込み専用の子プロセスを作る！
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		// エラー時は作ったパイプを閉じて帰る
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}

	if (pid == 0) // === 【子プロセスの仕事】 ===
        exec_heredoc_child(pipefd, delimiter);
    
    // === 【親プロセスの仕事】 ===
    return wait_heredoc_parent(pid, pipefd);
}

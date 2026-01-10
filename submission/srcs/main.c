#include "minishell.h"
#include "libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

// ft_split で確保した二次元配列を解放する関数
void    free_array(char **arr)
{
    int i;

    i = 0;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}

// [一時的] トークンリストを char **argv に変換する関数
// Parserを作るまでの繋ぎとして、exec_simple_cmd を再利用するために使う
char	**token_list_to_argv(t_token *tokens)
{
	int		count;
	t_token	*tmp;
	char	**argv;
	int		i;

	// 1. 要素数を数える
	count = 0;
	tmp = tokens;
	while (tmp && tmp->kind != TK_EOF) // EOFトークンは含めない
	{
		count++;
		tmp = tmp->next;
	}

	// 2. 配列確保
	argv = malloc(sizeof(char *) * (count + 1));
	if (!argv)
		return (NULL);

	// 3. 文字列をコピー (strdup必須)
	i = 0;
	tmp = tokens;
	while (tmp && tmp->kind != TK_EOF)
	{
		argv[i] = ft_strdup(tmp->word); // ここでコピーしないと後でdouble freeになる
		tmp = tmp->next;
		i++;
	}
	argv[i] = NULL;
	return (argv);
}

/*
** ループ処理を行う関数
** readlineはmallocされた文字列を返すので、使い終わったらfreeが必要です。
** lineがNULLの場合は、Ctrl-D (EOF) が入力されたことを意味します。
*/
void minishell_loop(char **environ){
    char *line;
    t_token	*token_list;
    char **args;

    while (1)
    {
        g_signal = 0; // シグナルステータスをリセット
        line = readline(GREEN"minishell> "RESET);
        if(line == NULL)
            break;

        // シグナルが発生した場合は継続
        if (g_signal == SIGINT)
        {
            free(line);
            continue;
        }

        // 1. 読み込み
        if (*line){
            add_history(line);
        }

        // 2. 字句解析 (Lexer)
		token_list = tokenize(line);
		
		// [Debug] トークンの中身を見てみる (開発中はこれをコメントアウトして確認)
		// t_token *curr = token_list;
		// while (curr) {
		// 	printf("Token: kind=%d, word=[%s]\n", curr->kind, curr->word);
		// 	curr = curr->next;
		// }
		

        // 3. 実行 (Executor)
		// ※ パイプなどはまだ動かない。単純コマンドのみ。
		if (token_list && token_list->kind != TK_EOF)
		{
			args = token_list_to_argv(token_list);
			execute_command(args, environ); // コマンド実行関数（未実装）
			free_array(args);
		}

        // 4. 後始末
		token_free(&token_list); // リスト全体を解放

        free(line);//readlienで確保したメモリを解放
    }
}


int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    setup_signals();
    minishell_loop(envp);
    return (0);
}

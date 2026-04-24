#include "minishell.h"
#include "libft.h"

// 1. ループで文字を読み進め、トークンリストを構築する
static int	process_tokens(t_token **head, char **line)
{
	while (**line)
	{
		if (is_space(**line))
			consume_space(line);
		else if (is_metachar(**line))
		{
			if (!append_operator(head, line))
				return (0);
		}
		else
		{
			if (!append_word(head, line))
				return (0);
		}
	}
	return (1);
}

// 2. 最後に EOF トークンを追加する。malloc失敗時は全破棄する
static t_token	*append_eof(t_token **head)
{
	t_token	*eof_token;

	eof_token = token_new(NULL, TK_EOF);
	if (!eof_token)
	{
		token_free(head);
		return (NULL);
	}
	token_add_back(head, eof_token);
	return (*head);
}

/*
** 入力文字列 line をトークンに分割し、連結リストとして返す。
** 以下の順で処理する：
**   1. クォートの閉じ忘れチェック（エラーなら NULL を返す）
**   2. スペースを読み飛ばす
**   3. メタ文字（| < >）はオペレータトークンとして追加
**   4. それ以外は単語トークンとして追加
**   5. 終端に TK_EOF トークンを追加して返す
*/
t_token *tokenize(char *line)
{
	t_token *head; // リストの先頭

	head = NULL;
	if(line == NULL)
		return NULL;
	if (has_unclosed_quote(line)) // 閉じ忘れクォートがあればエラー
	{
		ft_putendl_fd("minishell: syntax error: unclosed quote",
			STDERR_FILENO);
		return (NULL); // NULL を返すと minishell_loop 側でエラー扱いになる
	}
	if (!process_tokens(&head, &line))
	{
		token_free(&head);
		return (NULL);
	}
	// 4. 最後にEOFトークンを追加しておくと便利
	return (append_eof(&head));
}

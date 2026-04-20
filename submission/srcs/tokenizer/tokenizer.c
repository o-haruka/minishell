#include "minishell.h"
#include "libft.h"


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
	while (*line)
	{
		// 1. スペース読み飛ばし
		// TODO is_spaceはlibftのft_isspaceの方がいいか？
		if (is_space(*line))
			consume_space(&line);

		// 2. メタ文字（| < >）の処理
		else if (is_metachar(*line))
			append_operator(&head, &line);

		// 3. 一般単語（文字, クォート含む）の処理
		else
			append_word(&head, &line);
	}
	// 4. 最後にEOFトークンを追加しておくと便利
	token_add_back(&head, token_new(NULL, TK_EOF));
	return (head);
}

#include "minishell.h"

t_token *tokenize(char *line)
{
	t_token *head; // リストの先頭
	head = NULL;

	if(line == NULL)
		return NULL;

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

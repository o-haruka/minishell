#include "minishell.h"

// ポインタを進める処理
// クォート内のスペースやメタ文字を単語の区切りとして扱わないために、クォートの開閉状態を追跡する。
// 例: "hello world" → スペースを無視して1トークンにする
static void	advance_word_ptr(char **line)
{
	char	quote;

	quote = 0; // 0は「クォートに入っていない」状態
	while (**line)
	{
		if (quote && **line == quote) // クォートの中の処理 && 閉じクォートが見つかったら
			quote = 0;
		else if (!quote)
		{
			if (is_space(**line) || is_metachar(**line))
				break ;
			if (**line == '\'' || **line == '\"')
				quote = **line;
		}
		(*line)++;
	}
}

// 単語の終了位置までポインタを進め、トークンを作成する
int append_word(t_token **head, char **line)
{
    char *start;
    char *word_str;
    t_token *new_token;

    start = *line; // 1. 開始位置をメモ
    advance_word_ptr(line);

    // 2. 切り出し (start から 現在の *line まで)
    // ※ ft_substr(文字列, 開始インデックス, 長さ)
    //文字列の一部を切り出して新しい文字列を作成する関数（substring = 部分文字列）。
    // ここではポインタの引き算で長さを出しています
    //!ft_substrはmalloc使用。どこでfreeする？
    word_str = ft_substr(start, 0, *line - start);
    if(!word_str)
        return (0);

    // 3. リストに追加
    new_token = token_new(word_str, TK_WORD);
    if (!new_token) {
        free(word_str);
        return (0);
    }
    token_add_back(head, new_token);
    return (1);
}

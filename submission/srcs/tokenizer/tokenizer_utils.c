#include "minishell.h"
//! libftのft_isspaceを使うべき？？
bool is_space(char c) {
	return (c == ' ' || c == '\t');
}

// メタ文字 (区切り文字) かどうか
// シェルにとって特別な意味を持つ文字(必須課題): | < >
//! | < >だけでいいのか？
bool is_metachar(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

void consume_space(char **line) {
	while (**line && is_space(**line))
		(*line)++;
}

/*
** クォート（' または "）が正しく閉じられているか確認する。
** 閉じられていない場合は 1 を返す。
** 例:
**   echo 'hello'  → 0（正常）
**   echo 'hello   → 1（エラー: シングルクォートが閉じていない）
**   echo "hello  → 1（エラー: ダブルクォートが閉じていない）
*/
int	has_unclosed_quote(char *line)
{
	char	quote;

	quote = 0; // 0 = 現在クォートの外にいる
	while (*line)
	{
		if (quote) // クォートの中にいる
		{
			if (*line == quote) // 開いたクォートと同じ文字が来たら
				quote = 0;      // クォートを閉じる（外に出る）
		}
		else // クォートの外にいる
		{
			if (*line == '\'' || *line == '"')
				quote = *line; // クォートを開く（中に入る）
		}
		line++;
	}
	return (quote != 0); // まだ quote が残っている = 閉じ忘れ
}

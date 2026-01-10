#include "minishell.h"
bool is_space(char c) {
	return (c == ' ' || c == '\t');
}

// メタ文字 (区切り文字) かどうか
// シェルにとって特別な意味を持つ文字(必須課題): | < >
bool is_metachar(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

void consume_space(char **line) {
	while (**line && is_space(**line))
		(*line)++;
}

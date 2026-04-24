#include "minishell.h"
#include "libft.h"

// malloc失敗時は 0 を返し、成功時は 1 を返す
// 新しいノードを作ってリストに繋ぐ
static int	add_operator_token(t_token **head, char *operator_str, t_token_kind kind)
{
	t_token	*new_token;

	if (!operator_str)
		return (0);
	new_token = token_new(operator_str, kind);
	if (!new_token)
	{
		free(operator_str);
		return (0);
	}
	token_add_back(head, new_token);
	return (1);
}

// 演算子トークンを作成し、リストに追加し、入力読み込み位置を進める
static int append_pipe(t_token **head, char **line)
{
	char *operator_str;
	(*line)++; // 1文字進める
	operator_str = ft_strdup("|");
    if(operator_str == NULL)
        return (0);
    return (add_operator_token(head, operator_str, TK_PIPE));
}

static int	append_redirect_in(t_token **head, char **line)
{
	char	*operator_str;

	if ((*line)[1] == '<') // 次の文字を「先読み」 //「現在の場所から見て何番目か？」を表している
	{
		(*line) += 2; // 2文字進める
		operator_str = ft_strdup("<<");
        if(operator_str == NULL)
            return (0);
		return (add_operator_token(head, operator_str, TK_HEREDOC));
	}
	(*line)++;
	operator_str = ft_strdup("<");
	if (operator_str == NULL)
	{
		return (0);
	}
	return (add_operator_token(head, operator_str, TK_REDIRECT_IN));
}

static int	append_redirect_out(t_token **head, char **line)
{
	char	*operator_str;

	if ((*line)[1] == '>') // 次の文字を「先読み」 //「現在の場所から見て何番目か？」を表している
	{
		(*line) += 2; // 2文字進める
		operator_str = ft_strdup(">>");
        if(operator_str == NULL)
            return (0);
		return (add_operator_token(head, operator_str, TK_APPEND));
	}
	(*line)++;
	operator_str = ft_strdup(">");
	if (operator_str == NULL)
		return (0);
	return (add_operator_token(head, operator_str, TK_REDIRECT_OUT));
}

int append_operator(t_token **head, char **line)
{
    if (**line == '|')
		return (append_pipe(head, line));
    else if (**line == '>')
        return (append_redirect_out(head, line));
    else if (**line == '<')
        return (append_redirect_in(head, line));

    return (1);
}

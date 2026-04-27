#include "minishell.h"

/*
** 構文エラー時にエラーメッセージを出力して -1 を返すヘルパー。
** token が NULL か TK_EOF の場合は "newline" を、
** それ以外は実際のトークン文字列を表示する。
*/
static int	print_syntax_err(t_token *tok)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `",
		STDERR_FILENO);
	if (tok == NULL || tok->kind == TK_EOF)
		ft_putstr_fd("newline", STDERR_FILENO);
	else
		ft_putstr_fd(tok->word, STDERR_FILENO);
	ft_putendl_fd("'", STDERR_FILENO);
	return (-1);
}

/*
** リダイレクトトークンの直後を検証する。
** 直後が WORD 以外（EOF、パイプ、別のリダイレクト）なら構文エラー。
*/
static int	check_redir(t_token *tok)
{
	t_token	*next;

	next = tok->next;
	if (next == NULL || next->kind != TK_WORD)
		return (print_syntax_err(next));
	return (0);
}

/*
** パイプトークンの前後を検証する。
** ・先頭に | がある（prev == NULL）
** ・| の直後が | か EOF
*/
static int	check_pipe(t_token *prev, t_token *tok)
{
	t_token	*next;

	if (prev == NULL)
		return (print_syntax_err(tok));
	next = tok->next;
	if (next == NULL || next->kind == TK_EOF || next->kind == TK_PIPE)
		return (print_syntax_err(next));
	return (0);
}

/*
** トークン列全体を走査して構文エラーを検出する。
** 戻り値: 0 = 問題なし / -1 = 構文エラー
**
** チェック内容：
**   1. パイプ関連エラー（先頭パイプ、連続パイプ）
**   2. リダイレクト後のトークン欠落
*/
int	ft_check_syntax(t_token *tokens)
{
	t_token	*cur;
	t_token	*prev;

	prev = NULL;
	cur = tokens;
	while (cur != NULL && cur->kind != TK_EOF)
	{
		if (cur->kind == TK_PIPE)
		{
			if (check_pipe(prev, cur) == -1)
				return (-1);
		}
		else if (cur->kind == TK_REDIRECT_IN
			|| cur->kind == TK_REDIRECT_OUT
			|| cur->kind == TK_APPEND
			|| cur->kind == TK_HEREDOC)
		{
			if (check_redir(cur) == -1)
				return (-1);
		}
		prev = cur;
		cur = cur->next;
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirect.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:33:34 by homura            #+#    #+#             */
/*   Updated: 2026/04/29 21:52:42 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define REDIR_OK 1
#define REDIR_ERR 0
#define REDIR_SYNTAX -1

// ---------------------------------------------------------
// 2. リダイレクト構造体(t_redir)の初期化関数
// ---------------------------------------------------------
static t_redir	*init_redir_struct(void)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	*redir = (t_redir){0, NULL, -1, NULL};
	return (redir);
}

// ---------------------------------------------------------
// 3. コマンドのリダイレクトリストに新しい要素を追加する関数
// ---------------------------------------------------------
static void	add_redir_to_list(t_cmd *cmd, t_redir *new_redir)
{
	t_redir	*last;

	if (cmd->redirs == NULL)
		cmd->redirs = new_redir;
	else
	{
		last = cmd->redirs;
		while (last->next != NULL)
			last = last->next;
		last->next = new_redir;
	}
}

// t_redir 構造体を作って繋ぐ
// ---------------------------------------------------------
// 1. リダイレクトトークンを解析し、リストに追加する関数
// ---------------------------------------------------------
int	handle_redirection(t_cmd *cmd, t_token **current)
{
	t_redir	*new_redir;

	new_redir = init_redir_struct();
	if (!new_redir)
		return (free_cmd(cmd), REDIR_ERR);
	new_redir->kind = (*current)->kind;
	*current = (*current)->next;
	if (*current != NULL && (*current)->kind == TK_WORD)
	{
		new_redir->file = ft_strdup((*current)->word);
		if (!new_redir->file)
			return (free(new_redir), free_cmd(cmd), REDIR_ERR);
		*current = (*current)->next;
	}
	else
	{
		ft_putendl_fd("minishell: syntax error near unexpected token",
			STDERR_FILENO);
		return (free(new_redir), free_cmd(cmd), REDIR_SYNTAX);
	}
	add_redir_to_list(cmd, new_redir);
	return (REDIR_OK);
}

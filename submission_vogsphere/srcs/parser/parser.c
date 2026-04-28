/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:33:21 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 16:46:10 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/********************************************************
minishellパーサ：コマンド構造体リストを構築する主要関数群
*********************************************************/
#include "minishell.h"

// ---------------------------------------------------------
// 3. コマンド構造体(t_cmd)の初期化関数
// ---------------------------------------------------------
t_cmd	*init_cmd_struct(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

// ---------------------------------------------------------
// 4. コマンドの引数数をカウントする関数
// ---------------------------------------------------------
int	count_words(t_token *current)
{
	int	count;

	count = 0;
	while (current != NULL && current->kind != TK_EOF
		&& current->kind != TK_PIPE)
	{
		if (current->kind == TK_WORD)
		{
			count++;
			current = current->next;
		}
		else if (current->kind == TK_REDIRECT_IN
			|| current->kind == TK_REDIRECT_OUT || current->kind == TK_APPEND
			|| current->kind == TK_HEREDOC)
		{
			current = current->next;
			if (current != NULL && current->kind == TK_WORD)
				current = current->next;
		}
		else
			current = current->next;
	}
	return (count);
}

// ---------------------------------------------------------
// 5. コマンドトークンを解析し、args/redirsに格納する関数
// ---------------------------------------------------------
static int	process_cmd_tokens(t_cmd *cmd, t_token **current)
{
	int	i;
	int	redir_result;

	i = 0;
	while (*current != NULL && (*current)->kind != TK_EOF
		&& (*current)->kind != TK_PIPE)
	{
		if ((*current)->kind == TK_WORD)
		{
			cmd->args[i] = ft_strdup((*current)->word);
			if (!cmd->args[i])
				return (free_cmd(cmd), 0);
			i++;
			*current = (*current)->next;
		}
		else
		{
			redir_result = handle_redirection(cmd, current);
			if (redir_result <= 0)
				return (redir_result);
		}
	}
	cmd->args[i] = NULL;
	return (1);
}

// ---------------------------------------------------------
// 2. 1コマンド分（パイプ区切り）を組み立てる関数
// ---------------------------------------------------------
t_cmd	*parse_command(t_token **current)
{
	t_cmd	*cmd;
	int		num_words;
	int		status;

	cmd = init_cmd_struct();
	if (cmd == NULL)
		return (NULL);
	num_words = count_words(*current);
	cmd->args = malloc(sizeof(char *) * (num_words + 1));
	if (!cmd->args)
		return (free_cmd(cmd), NULL);
	ft_memset(cmd->args, 0, sizeof(char *) * (num_words + 1));
	status = process_cmd_tokens(cmd, current);
	if (status <= 0)
		return (NULL);
	return (cmd);
}

// ---------------------------------------------------------
// 1. 全体のパースを統括するエントリー関数
// ---------------------------------------------------------
t_cmd	*parse(t_token *tokens)
{
	t_cmd	*cmd_head;
	t_cmd	*cmd_tail;
	t_cmd	*new_cmd;
	t_token	*current_token;

	cmd_head = NULL;
	cmd_tail = NULL;
	new_cmd = NULL;
	current_token = tokens;
	while (current_token != NULL && current_token->kind != TK_EOF)
	{
		new_cmd = parse_command(&current_token);
		if (!new_cmd)
			return (free_cmds_list(cmd_head), NULL);
		if (cmd_head == NULL)
			cmd_head = new_cmd;
		else
			cmd_tail->next = new_cmd;
		cmd_tail = new_cmd;
		if (current_token != NULL && current_token->kind == TK_PIPE)
			current_token = current_token->next;
	}
	return (cmd_head);
}

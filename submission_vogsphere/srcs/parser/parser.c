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

#include "minishell.h"

/*
** Allocate and zero-initialize a t_cmd node.
** Called by parse_command for each pipe-separated command.
*/
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

/*
** Count WORD tokens in the current command segment (up to TK_PIPE or TK_EOF),
** skipping redirection operator/filename pairs.
** Used to size the args array in parse_command.
*/
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

/*
** Fill cmd->args and cmd->redirs by consuming tokens until TK_PIPE or TK_EOF.
** Returns 1 on success, 0 on allocation error, -1 on syntax error.
*/
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

/*
** Build one t_cmd from the token stream up to the next TK_PIPE or TK_EOF.
** Allocates args sized by count_words, then fills it via process_cmd_tokens.
** Called by parse for each pipe-separated segment.
*/
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

/*
** Entry point for parsing. Converts a token list into a t_cmd linked list.
** Calls parse_command once per pipe-separated segment.
** Returns the head of the command list, or NULL on error.
*/
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

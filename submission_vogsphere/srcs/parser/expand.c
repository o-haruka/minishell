/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:33:37 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/30 17:06:36 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Read the variable name after '$' and return its expanded value.
** $? → last exit status; $NAME → env value; bare $ → "$"; undefined → "".
** Returns a newly allocated string; the caller is responsible for freeing it.
*/
static char	*ft_get_dollar_value(char *str, int *i, t_shell *shell)
{
	int		len;
	char	*value;

	*i += 1;
	if (str[*i] == '?')
	{
		*i += 1;
		return (ft_itoa(shell->last_status));
	}
	len = ft_get_var_len(str + *i);
	if (len == 0)
		return (ft_strdup("$"));
	value = ft_find_env(shell->env, str + *i, len);
	*i += len;
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

/*
** Expand one '$' token and append the result to *res.
** Takes char ** so the caller's pointer can be updated by ft_append_expanded.
** Returns 0 on success, -1 on allocation failure (also frees and NULLs *res).
*/
static int	ft_handle_dollar(char **res, char *str, int *i, t_shell *shell)
{
	char	*part;

	part = ft_get_dollar_value(str, i, shell);
	if (!part)
	{
		free(*res);
		*res = NULL;
		return (-1);
	}
	*res = ft_append_expanded(*res, part);
	if (!*res)
		return (-1);
	return (0);
}

/*
** Toggle the single- or double-quote flag when a quote character is seen,
** then advance i past it. The quote character itself is not added to result.
*/
static void	ft_toggle_quote(char c, bool *in_sq, bool *in_dq, int *i)
{
	if (c == '\'')
		*in_sq = !*in_sq;
	else
		*in_dq = !*in_dq;
	*i += 1;
}

/*
** Scan str one character at a time and return a fully expanded string.
** Single-quoted text is copied verbatim; '$' outside single quotes is expanded.
** Quote characters themselves are stripped from the output.
*/
static char	*ft_expand_str(char *str, t_shell *shell)
{
	char	*result;
	int		i;
	bool	in_sq;
	bool	in_dq;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	in_sq = false;
	in_dq = false;
	while (str[i])
	{
		if ((str[i] == '\'' && !in_dq) || (str[i] == '"' && !in_sq))
			ft_toggle_quote(str[i], &in_sq, &in_dq, &i);
		else if (str[i] != '$' || in_sq)
			result = ft_append_char(result, str, &i);
		else if (ft_handle_dollar(&result, str, &i, shell) != 0)
			return (NULL);
		if (!result)
			return (NULL);
	}
	return (result);
}

/*
** Entry point for expansion. Walks all commands in shell->cmds and replaces
** each arg with its quote-stripped, variable-expanded form.
** Called after parsing, before execution.
*/
void	ft_expand_args(t_shell *shell)
{
	t_cmd	*cmd;
	char	*expanded;
	int		i;

	cmd = shell->cmds;
	while (cmd)
	{
		i = 0;
		while (cmd->args[i])
		{
			expanded = ft_expand_str(cmd->args[i], shell);
			if (!expanded)
			{
				shell->last_status = 1;
				return ;
			}
			free(cmd->args[i]);
			cmd->args[i] = expanded;
			i++;
		}
		cmd = cmd->next;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:57 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:44:15 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Remove the node matching key from the env list and free it.
** Updates the head pointer when the first node is removed.
*/
static void	ft_unset_var(t_shell *shell, char *key)
{
	t_env	*current;
	t_env	*prev;

	prev = NULL;
	current = shell->env;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
		{
			if (prev == NULL)
				shell->env = current->next;
			else
				prev->next = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

/*
** Builtin unset: remove each argument from the env list.
*/
int	ft_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;

	i = 1;
	while (cmd->args[i])
	{
		ft_unset_var(shell, cmd->args[i]);
		i++;
	}
	return (0);
}

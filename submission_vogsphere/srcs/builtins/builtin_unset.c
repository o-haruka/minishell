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
** env リストから key が一致するノードを1つ削除する。
** 削除の仕組み：
**   削除前: [prev] → [current] → [next]
**   削除後: [prev] → [next]
**   ※ prev->next を current->next に繋ぎ替えて current を free する
** 先頭ノードが対象の場合は shell->env 自体を次に付け替える。
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
** unset ビルトインの実装。
** 引数を複数受け取り、それぞれを env リストから削除する。
**
** 例) unset NAME USER PATH
**   → args[0]="unset", args[1]="NAME", args[2]="USER", args[3]="PATH"
**   → NAME, USER, PATH を順番に env リストから削除する
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

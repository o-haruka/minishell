/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_envp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 00:00:00 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/29 00:00:00 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** env_to_envp で作った char** 配列を解放する。
** 各文字列（"KEY=VALUE"）と配列本体を両方 free する。
*/
void	free_envp(char **envp)
{
	int	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
		free(envp[i++]);
	free(envp);
}

/*
** t_env の1ノードから "KEY=VALUE" 形式の文字列を作って返す。
** value が NULL のノード（export NAME のみで登録された変数）は
** "KEY=" として扱う。
*/
static char	*make_kv_pair(t_env *node)
{
	char	*tmp_str;
	char	*kv;
	char	*val;

	tmp_str = ft_strjoin(node->key, "=");
	if (!tmp_str)
		return (NULL);
	val = node->value;
	if (!val)
		val = "";
	kv = ft_strjoin(tmp_str, val);
	free(tmp_str);
	return (kv);
}

/*
** t_env リストを走査して envp 配列を埋める。
** 失敗時は作成済みの要素を解放する（envp 配列自体は呼び出し元が解放する）。
*/
static int	fill_envp(t_env *env, char **envp)
{
	char	*kv;
	int		i;

	i = 0;
	while (env)
	{
		kv = make_kv_pair(env);
		if (!kv)
		{
			while (i > 0)
				free(envp[--i]);
			return (0);
		}
		envp[i] = kv;
		i++;
		env = env->next;
	}
	envp[i] = NULL;
	return (1);
}

/*
** t_env リストを char **envp 形式に変換して返す。
** execve に渡すために使う。
** 使い終わったら free_envp() で解放すること。
*/
char	**env_to_envp(t_env *env)
{
	char	**envp;
	t_env	*tmp;
	int		count;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	envp = malloc(sizeof(char *) * (count + 1));
	if (!envp)
		return (NULL);
	if (!fill_envp(env, envp))
	{
		free(envp);
		return (NULL);
	}
	return (envp);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:33:26 by hkuninag          #+#    #+#             */
/*   Updated: 2026/04/28 16:44:59 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Search the env list for a key matching exactly len characters.
** Both prefix and length are checked to avoid matching "HOMEPATH" when
** looking up "HOME". Returns the value string, or NULL if not found.
*/
char	*ft_find_env(t_env *env, char *key, int len)
{
	while (env)
	{
		if (ft_strncmp(env->key, key, len) == 0
			&& (int)ft_strlen(env->key) == len)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** Count valid variable-name characters (alphanumeric or '_') from the start
** of s. Returns 0 if the first character is not a valid name character.
*/
int	ft_get_var_len(char *s)
{
	int	len;

	len = 0;
	while (s[len] && (ft_isalnum(s[len]) || s[len] == '_'))
		len++;
	return (len);
}

/*
** Append the single character at str[*i] to result, advance *i, and return
** the new string. Frees the old result to avoid memory leaks.
*/
char	*ft_append_char(char *result, char *str, int *i)
{
	char	*tmp;
	char	*joined;

	tmp = ft_substr(str, *i, 1);
	if (!tmp)
	{
		free(result);
		return (NULL);
	}
	joined = ft_strjoin(result, tmp);
	free(result);
	free(tmp);
	*i += 1;
	return (joined);
}

/*
** Concatenate part onto result, free both, and return the new string.
** Used to build up the expanded output without leaking intermediate buffers.
*/
char	*ft_append_expanded(char *result, char *part)
{
	char	*joined;

	joined = ft_strjoin(result, part);
	free(result);
	free(part);
	return (joined);
}

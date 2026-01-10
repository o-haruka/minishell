/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 02:32:17 by vscode            #+#    #+#             */
/*   Updated: 2025/11/08 18:41:16 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

static size_t	count_words(char const *s, char c)
{
	size_t	count;

	count = 0;
	while (*s)
	{
		if (*s != c)
		{
			count++;
			while (*s && *s != c)
				s++;
		}
		else
			s++;
	}
	return (count);
}

static char	*extract_word(char const *s, size_t *i, char c)
{
	char	*word;
	size_t	start;
	size_t	len;

	start = *i;
	while (s[*i] && s[*i] != c)
		(*i)++;
	len = *i - start;
	word = (char *)malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	while (len--)
		word[len] = s[start + len];
	word[*i - start] = '\0';
	return (word);
}

static void	free_array(char **array, size_t index)
{
	while (index--)
		free(array[index]);
	free(array);
}

char	**ft_split(char const *s, char c)
{
	char	**result;
	size_t	i;
	size_t	j;

	if (!s)
		return (NULL);
	result = (char **)malloc(sizeof(char *) * (count_words(s, c) + 1));
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			result[j++] = extract_word(s, &i, c);
			if (!result[j - 1])
				return (free_array(result, j), NULL);
		}
		else
			i++;
	}
	result[j] = NULL;
	return (result);
}

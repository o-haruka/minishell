/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:16:12 by homura            #+#    #+#             */
/*   Updated: 2025/08/12 18:47:32 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_free(char *s)
{
	free(s);
	return (NULL);
}

int	ft_getc(int fd)
{
	static char	buf[BUFFER_SIZE];
	static char	*p;
	static int	n = 0;

	if (n == 0)
	{
		n = read(fd, buf, BUFFER_SIZE);
		if (n < 0)
		{
			n = 0;
			return (-2);
		}
		if (n == 0)
			return (EOF);
		p = buf;
	}
	if (n > 0)
	{
		n--;
		return ((unsigned char)*p++);
	}
	return (EOF);
}

int	ft_putc(t_string *s, char c)
{
	size_t	new_capa;
	char	*new_str;
	size_t	i;

	if (s->len + 1 > s->capa)
	{
		if (s->capa == 0)
			new_capa = 16;
		else
			new_capa = s->capa * 2;
		new_str = malloc(new_capa);
		if (!new_str)
			return (-1);
		i = 0;
		while (i < s->len)
		{
			new_str[i] = s->str[i];
			i++;
		}
		free(s->str);
		s->str = new_str;
		s->capa = new_capa;
	}
	s->str[s->len++] = c;
	return (0);
}

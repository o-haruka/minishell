/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:16:02 by homura            #+#    #+#             */
/*   Updated: 2026/04/26 16:52:50 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	t_string	ret;
	int			c;

	ret = (t_string){NULL, 0, 0};
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	while (1)
	{
		c = ft_getc(fd);
		if (c == -2)
			return (ft_free(ret.str));
		if (c == EOF)
			break ;
		if (ft_putc(&ret, c) == -1)
			return (ft_free(ret.str));
		if (c == '\n')
			break ;
	}
	if (ret.len == 0)
		return (ft_free(ret.str));
	if (ft_putc(&ret, '\0') == -1)
		return (ft_free(ret.str));
	return (ret.str);
}

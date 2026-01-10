/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:16:02 by homura            #+#    #+#             */
/*   Updated: 2025/08/12 20:51:21 by homura           ###   ########.fr       */
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

#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int fd;
	char *line;
	int line_count;
	line_count = 0;

	fd = open("text.txt", O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return (1);
	}

	printf("ファイルの内容を1行ずつ読み込みます:\n");
	printf("========================================\n");

	while ((line = get_next_line(fd)) != NULL)
	{
		line_count++;
		printf("行 %d: %s", line_count, line);

		free(line);
	}

	printf("\n========================================\n");
	printf("読み込み完了。総行数: %d行\n", line_count);

	close(fd);
	return (0);
}
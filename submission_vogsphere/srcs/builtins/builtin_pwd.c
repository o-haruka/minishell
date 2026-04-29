/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:50 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 17:12:23 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//?Macや最近のLinuxの getcwd には、「裏技（拡張仕様）」が用意されている。
//第1引数に NULL、第2引数に 0 を渡すと、getcwd 自身が必要な文字数を計算し、
// 勝手にmallocでピッタリの箱を作ってパスを入れ、そのポインタを返してくれます。

// ?man 3  getpwd
// If buf is NULL,
// space is allocated as necessary to store the pathname and size is ignored.
// This space may later be free(3)'d.

int	ft_pwd(void)
{
	char	*path;

	path = getcwd(NULL, 0);
	if (path != NULL)
	{
		ft_putendl_fd(path, STDOUT_FILENO);
		free(path);
		return (0);
	}
	else
	{
		print_error_msg(NULL, "pwd", strerror(errno));
		return (1);
	}
}

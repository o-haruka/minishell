/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isint.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:50:04 by homura            #+#    #+#             */
/*   Updated: 2025/11/08 19:01:01 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_string.h"
#include "../includes/libft.h"

/*
** 文字列が有効な整数（int型の範囲内）かチェックする
** 戻り値: 1(有効) / 0(無効)
** - 符号（+/-）の確認
** - 数字以外の文字が含まれていないかチェック
** - INT_MIN <= num <= INT_MAX の範囲内かを確認
*/
int	ft_isint(char *str)
{
	long	num;
	size_t	i;

	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i] != '\0')
	{
		if (ft_isdigit(str[i]) == 0)
			return (0);
		i++;
	}
	num = ft_strtol(str);
	if (num < INT_MIN || INT_MAX < num)
		return (0);
	return (1);
}

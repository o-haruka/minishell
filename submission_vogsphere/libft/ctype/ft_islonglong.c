/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_islonglong.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 00:00:00 by copilot           #+#    #+#             */
/*   Updated: 2026/04/28 17:03:49 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_ctype.h"

static int	ft_parse_sign(char *str, size_t *i, int *sign)
{
	*i = 0;
	*sign = 1;
	if (str[*i] == '-' || str[*i] == '+')
	{
		if (str[*i] == '-')
			*sign = -1;
		(*i)++;
	}
	if (str[*i] == '\0')
		return (0);
	return (1);
}

static int	ft_check_longlong_range(char *str, size_t i, int sign)
{
	long long	num;
	int			digit;

	num = 0;
	while (str[i] != '\0')
	{
		if (ft_isdigit(str[i]) == 0)
			return (0);
		digit = str[i] - '0';
		if (sign == 1)
		{
			if (num > (LLONG_MAX - digit) / 10)
				return (0);
			num = num * 10 + digit;
		}
		else
		{
			if (num < (LLONG_MIN + digit) / 10)
				return (0);
			num = num * 10 - digit;
		}
		i++;
	}
	return (1);
}

/*
** 文字列が有効な long long かチェックする
** 戻り値: 1(有効) / 0(無効)
*/
int	ft_islonglong(char *str)
{
	size_t	i;
	int		sign;

	if (ft_parse_sign(str, &i, &sign) == 0)
		return (0);
	return (ft_check_longlong_range(str, i, sign));
}

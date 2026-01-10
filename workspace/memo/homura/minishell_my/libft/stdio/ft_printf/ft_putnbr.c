/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 17:22:18 by homura            #+#    #+#             */
/*   Updated: 2025/06/24 13:49:27 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_nbr(int n)
{
	int		len;
	long	num;
	char	digit;

	len = 0;
	if (n == 0)
	{
		ft_putchar('0');
		return (1);
	}
	num = n;
	if (num < 0)
	{
		ft_putchar('-');
		len++;
		num = -num;
	}
	if (num >= 10)
		len += ft_print_nbr(num / 10);
	digit = '0' + (num % 10);
	ft_putchar(digit);
	len++;
	return (len);
}

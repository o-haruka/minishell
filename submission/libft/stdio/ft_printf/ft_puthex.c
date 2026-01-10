/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 17:22:05 by homura            #+#    #+#             */
/*   Updated: 2025/06/24 13:47:23 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_puthex_lower(unsigned int nb)
{
	unsigned int	len;

	len = 0;
	if (nb >= 16)
	{
		len += ft_puthex_lower(nb / 16);
		nb %= 16;
	}
	if (nb < 10)
		len += ft_putchar(nb + '0');
	else
		len += ft_putchar(nb - 10 + 'a');
	return (len);
}

int	ft_puthex_upper(unsigned int nb)
{
	unsigned int	len;

	len = 0;
	if (nb >= 16)
	{
		len += ft_puthex_upper(nb / 16);
		nb %= 16;
	}
	if (nb < 10)
		len += ft_putchar(nb + '0');
	else
		len += ft_putchar(nb - 10 + 'A');
	return (len);
}

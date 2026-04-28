/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 17:22:59 by homura            #+#    #+#             */
/*   Updated: 2025/06/24 13:48:44 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_puthex_ptr(uintptr_t nb)
{
	uintptr_t	len;

	len = 0;
	if (nb >= 16)
	{
		len += ft_puthex_ptr(nb / 16);
		nb %= 16;
	}
	if (nb < 10)
		len += ft_putchar(nb + '0');
	else
		len += ft_putchar(nb - 10 + 'a');
	return (len);
}

int	ft_putptr(uintptr_t nb)
{
	int	len;

	len = 0;
	if (nb == 0)
		return (write(1, "(nil)", 5));
	len += write(1, "0x", 2);
	len += ft_puthex_ptr(nb);
	return (len);
}

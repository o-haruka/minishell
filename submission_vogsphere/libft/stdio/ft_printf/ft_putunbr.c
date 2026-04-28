/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putunbr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 17:23:15 by homura            #+#    #+#             */
/*   Updated: 2025/06/22 17:28:07 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putunbr(unsigned int n)
{
	int	len;
	int	digit;

	len = 0;
	if (n >= 10)
		len += ft_putunbr(n / 10);
	digit = '0' + (n % 10);
	if (write(1, &digit, 1) == -1)
		return (-1);
	len++;
	return (len);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 17:20:58 by homura            #+#    #+#             */
/*   Updated: 2025/06/24 16:47:07 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_conversion(char type, va_list args)
{
	int	len;

	len = 0;
	if (type == 'c')
		len += ft_putchar(va_arg(args, int));
	else if (type == 's')
		len += ft_putstr(va_arg(args, char *));
	else if (type == 'p')
		len += ft_putptr(va_arg(args, uintptr_t));
	else if (type == 'd' || type == 'i')
		len += ft_print_nbr(va_arg(args, int));
	else if (type == 'u')
		len += ft_putunbr(va_arg(args, unsigned int));
	else if (type == 'x')
		len += ft_puthex_lower(va_arg(args, unsigned int));
	else if (type == 'X')
		len += ft_puthex_upper(va_arg(args, unsigned int));
	else if (type == '%')
		len += write(1, "%", 1);
	return (len);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		printed_char;

	printed_char = 0;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			printed_char += ft_conversion(*format, args);
		}
		else
		{
			write(1, format, 1);
			printed_char++;
		}
		format++;
	}
	va_end(args);
	return (printed_char);
}

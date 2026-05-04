# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/09 20:36:41 by vscode            #+#    #+#              #
#    Updated: 2026/05/04 11:15:53 by homura           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDE = includes

CTYPE_SRC = \
ft_isalpha.c \
ft_isdigit.c \
ft_isalnum.c \
ft_isascii.c \
ft_isprint.c \
ft_toupper.c \
ft_tolower.c \
ft_isspace.c \
ft_isstrspace.c \
ft_isint.c \
ft_islonglong.c \

LIST_SRC = \
ft_lstnew.c \
ft_lstadd_front.c \
ft_lstsize.c \
ft_lstlast.c \
ft_lstadd_back.c \
ft_lstdelone.c \
ft_lstclear.c \
ft_lstiter.c \
ft_lstmap.c

STDIO_SRC = \
ft_putchar_fd.c \
ft_putstr_fd.c \
ft_putendl_fd.c \
ft_putnbr_fd.c \
ft_printf/ft_printf.c \
ft_printf/ft_putchar.c \
ft_printf/ft_puthex.c \
ft_printf/ft_putnbr.c \
ft_printf/ft_putptr.c \
ft_printf/ft_putstr.c \
ft_printf/ft_putunbr.c \
get_next_line/get_next_line.c \
get_next_line/get_next_line_utils.c \

STDLIB_SRC = \
ft_calloc.c \
ft_atoi.c \
ft_atoll.c \
ft_itoa.c \
ft_strtol.c \

STRING_SRC = \
ft_bzero.c \
ft_memset.c \
ft_memcpy.c \
ft_memmove.c \
ft_memchr.c \
ft_memcmp.c \
ft_split.c\
ft_strchr.c \
ft_strdup.c \
ft_striteri.c \
ft_strjoin.c \
ft_strlcat.c \
ft_strlcpy.c \
ft_strlen.c \
ft_strmapi.c \
ft_strncmp.c \
ft_strnstr.c \
ft_strrchr.c \
ft_strtrim.c \
ft_substr.c \

CTYPE_PATH = ctype
CTYPE_SRCS = $(addprefix $(CTYPE_PATH)/, $(CTYPE_SRC))

LIST_PATH = list
LIST_SRCS = $(addprefix $(LIST_PATH)/, $(LIST_SRC))

STDIO_PATH = stdio
STDIO_SRCS = $(addprefix $(STDIO_PATH)/, $(STDIO_SRC))

STDLIB_PATH = stdlib
STDLIB_SRCS = $(addprefix $(STDLIB_PATH)/, $(STDLIB_SRC))

STRING_PATH = string
STRING_SRCS = $(addprefix $(STRING_PATH)/, $(STRING_SRC))

SRCS = $(CTYPE_SRCS) \
		$(LIST_SRCS) \
		$(STDIO_SRCS) \
		$(STDLIB_SRCS) \
		$(STRING_SRCS)

OBJ = $(SRCS:%.c=%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@ ar rcs $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

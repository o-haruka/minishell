# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/28 16:36:27 by homura            #+#    #+#              #
#    Updated: 2026/05/05 09:57:23 by homura           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := minishell

CC          := cc
CFLAGS      := -Wall -Wextra -Werror

SRC_PATH    := srcs
OBJ_PATH    := objs
INC_PATH    := includes
LIB_PATH    := libft

SRCS        := main.c \
              executor/executor.c \
              executor/pipe.c \
			  executor/pipe_utils.c \
              executor/path.c \
			  executor/redirect.c \
			  executor/redirect_heredoc.c \
              executor/exec_utils.c \
              signal.c \
              tokenizer/tokenizer.c \
              tokenizer/tokenizer_utils.c \
              tokenizer/token_operator.c \
              tokenizer/token_word.c \
              tokenizer/token.c \
			  tokenizer/tokenizer_syntax.c \
              parser/parser.c \
              parser/parser_utils.c \
              parser/parser_redirect.c \
			  parser/expand.c \
			  parser/expand_utils.c \
			  env/env_list.c \
			  env/env_envp.c \
              env/env_update.c \
              builtins/builtin_utils.c \
              builtins/builtin_echo.c \
              builtins/builtin_pwd.c \
              builtins/builtin_env.c \
              builtins/builtin_cd.c \
              builtins/builtin_export.c \
              builtins/builtin_unset.c \
              builtins/builtin_exit.c \
              error.c

OBJ         := $(SRCS:.c=.o)
OBJS        := $(addprefix $(OBJ_PATH)/, $(OBJ))

INCS        := -I$(INC_PATH) -I$(LIB_PATH)/includes

LDFLAGS     := -L$(LIB_PATH)
LIBS        := -lreadline -lft

LIBFT       := $(LIB_PATH)/libft.a

UNAME       := $(shell uname)
ifeq ($(UNAME), Darwin)
    READLINE_PREFIX := $(shell brew --prefix readline)
    INCS    += -I$(READLINE_PREFIX)/include
    LDFLAGS += -L$(READLINE_PREFIX)/lib
endif

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIB_PATH)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_PATH)
	$(MAKE) -C $(LIB_PATH) clean

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIB_PATH) fclean

re: fclean all

.PHONY: all clean fclean re

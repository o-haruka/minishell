NAME        = minishell

CC          = cc
CFLAGS      = -Wall -Wextra -Werror

# --- Paths ---
SRC_PATH    = srcs
OBJ_PATH    = objs
INC_PATH    = includes
LIB_PATH    = libft

# --- Sources & Objects ---
SRCS        = main.c \
              executor/executor.c \
              executor/pipe.c \
			  executor/pipe_utils.c \
              executor/path.c \
			  executor/redirect.c \
			  executor/redirect_heredoc.c \
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
			  env/env_init.c \
			  env/env_utils.c \
              env/env_update.c \
              builtins/builtin_utils.c \
              builtins/builtin_echo.c \
              builtins/builtin_pwd.c \
              builtins/builtin_env.c \
              builtins/builtin_cd.c \
              builtins/builtin_export.c \
              builtins/builtin_unset.c \
              builtins/builtin_exit.c \
              terminal.c \
              debug.c \

# SRCSの拡張子 .c を .o に置換し、OBJ_PATH を前につける
# 例: srcs/tokenizer/token.c -> objs/tokenizer/token.o
OBJ         = $(SRCS:.c=.o)
OBJS        = $(addprefix $(OBJ_PATH)/, $(OBJ))

# --- Includes & Libraries ---
# ヘッダーファイルの場所指定
INCS        = -I$(INC_PATH) -I$(LIB_PATH)/includes

# ライブラリの場所(-L)とリンクするライブラリ名(-l)
LDFLAGS     = -L$(LIB_PATH)
LIBS        = -lreadline -lft

LIBFT       = $(LIB_PATH)/libft.a

# --- Platform Specific (macOS) ---
# macOSの場合のみ、Homebrewで入れたreadlineのパスを追加
UNAME       := $(shell uname)
ifeq ($(UNAME), Darwin)
    READLINE_PREFIX = $(shell brew --prefix readline)
    INCS    += -I$(READLINE_PREFIX)/include
    LDFLAGS += -L$(READLINE_PREFIX)/lib
endif

# --- Rules ---

all: $(NAME)

# 実行ファイルの生成
# リンク順序重要: オブジェクトファイル -> フラグ -> ライブラリ
$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(NAME)

# libftのコンパイル
$(LIBFT):
	$(MAKE) -C $(LIB_PATH)

# オブジェクトファイルの生成
# $< は依存ファイル(.c), $@ はターゲットファイル(.o)
# mkdir -p $(dir $@) で、必要なサブディレクトリ(objs/tokenizerなど)を自動生成
# libftを先にコンパイルしてからプロジェクトをコンパイルするために、libft.aに依存させる
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(LIBFT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -rf $(OBJ_PATH)
	$(MAKE) -C $(LIB_PATH) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIB_PATH) fclean

re: fclean all

.PHONY: all clean fclean re

#ifndef MINISHELL_H
# define MINISHELL_H
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <unistd.h>

# define CHECK "\033[32m[✔]\033[0m"
# define CROSS "\033[31m[✘]\033[0m"
# define GREEN "\033[0;32m"
# define RED "\033[0;31m"
# define RESET "\033[0m"

// グローバル変数宣言
extern int	g_signal;

/* --- Enum & Structs --- */

// トークンの種類(識別子)を表す列挙型enum
typedef enum e_token_kind
{
	TK_WORD,         // 一般的な文字列 (ls, -l, filename...)
	TK_PIPE,         // |
	TK_REDIRECT_IN,  // <
	TK_REDIRECT_OUT, // >
	TK_APPEND,       // >>
	TK_HEREDOC,      // <<
	TK_EOF           // 入力の終わり
}			t_token_kind;

// トークンを表す構造体
typedef struct s_token
{
	char *word;           // 実際の文字列 ("ls", "|", "file" など)
	t_token_kind kind;    // 文字の種類 (TK_WORD, TK_PIPE など)
	struct s_token *next; // 次のトークンへのポインタ
}			t_token;

/* --- Prototypes --- */

void		minishell_loop(char **environ);
void		setup_signals(void);

// tokenizer.c
t_token		*tokenize(char *line);

// token.c
t_token		*token_new(char *word, t_token_kind kind);
void		token_add_back(t_token **head, t_token *new_token);
void		token_free(t_token **head);

// lexer_utils.c
bool		is_space(char c);
bool		is_metachar(char c);
void		consume_space(char **line);

// token_operator.c
void		append_operator(t_token **head, char **line);
void		append_word(t_token **head, char **line);

// token_list_to_argv.c
char		**token_list_to_argv(t_token *head);
void		execute_command(char **cmdline, char **envp);

void		free_array(char **array);
char		*get_env_path(char **envp);
char		*join_path(char *dir, char *cmd);
char		*search_path(char *cmd, char **envp);

#endif /* MINISHELL_H */
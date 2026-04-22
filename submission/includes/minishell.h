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
# define PINK "\033[0;35m"
# define RESET "\033[0m"

// global
extern int	g_signal;

/***********************************
Enum & Structs
***********************************/

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

// リダイレクト情報（単方向リスト）
typedef struct s_redir {
    t_token_kind    kind;   // TK_REDIRECT_IN / TK_REDIRECT_OUT / TK_APPEND / TK_HEREDOC
    char            *file;  // ファイル名 or heredoc の区切り文字
    struct s_redir  *next;
} t_redir;

// 環境変数（単方向リスト）
typedef struct s_env {
    char            *key;   // "PATH" "HOME" など
    char            *value; // "/usr/bin:/bin" など
    struct s_env    *next;
} t_env;

// Parser が作るコマンド（単方向リスト）
typedef struct s_cmd {
    char            **args;  // {"ls", "-la", NULL} など
    t_redir         *redirs; // このコマンドのリダイレクト一覧
    struct s_cmd    *next;   // 次のコマンド（パイプの右側）
} t_cmd;


// 全体を束ねる管理構造体
typedef struct s_shell {
    t_env           *env;         // 環境変数リスト
    t_cmd           *cmds;        // パース済みコマンドリスト
    t_token         *tokens;      // Lexer が作ったトークンリスト
    int             last_status;  // $? の値
	char			**envp;       // mainが受け取る環境変数の文字列配列を、そのまま execve に渡すために保持
} t_shell;


/************************************
Prototypes
************************************/

/*----------------------------------------
main (main.c)
----------------------------------------*/
void		minishell_loop(t_shell *shell);

/*----------------------------------------
signal (signal.c)
----------------------------------------*/
void		setup_signals(void);
void		set_signal_for_parent_wait(void);
void		set_signal_for_child(void);

/*----------------------------------------
tokenizer
----------------------------------------*/
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
int			has_unclosed_quote(char *line);

// token_operator.c
int		append_operator(t_token **head, char **line);
int		append_word(t_token **head, char **line);

/*----------------------------------------
parser
----------------------------------------*/
// parser.c
t_cmd		*parse(t_token *tokens);

// parser_utils.c
void		free_cmd(t_cmd *cmd);
void		free_cmds_list(t_cmd *cmd);

// expand.c
void    ft_expand_args(t_shell *shell);

// expand_utils.c
char	*ft_find_env(t_env *env, char *key, int len);
int		ft_get_var_len(char *s);
char	*ft_append_char(char *result, char *str, int *i);
char	*ft_append_expanded(char *result, char *part);
char    *ft_get_dollar_value(char *str, int *i, t_shell *shell);

/*----------------------------------------
env
----------------------------------------*/
// env_init.c
t_env   *init_env(char **envp);

// env_utils.c
void    free_env(t_env *env);
char    *get_env_value(t_env *env, char *key);
char    **env_to_envp(t_env *env);
int update_env_value(t_env **env, char *key, char *new_value);

/*----------------------------------------
excecter
----------------------------------------*/
// executor.c
void    ft_execute(t_shell *shell);

// path.c
char *search_path(char *cmd, t_env *env);

// redirect.c
int     ft_apply_redirs(t_cmd *cmd);

// redirect_heredoc.c
int	apply_heredoc(char *delimiter);

// executor/pipe.c
void    ft_execute_pipeline(t_shell *shell);

// executor/pipe_utils.c
int     count_cmds(t_cmd *cmd);
int     open_all_pipes(int (*pipes)[2], int pipe_count);
void    close_all_pipes(int (*pipes)[2], int pipe_count);
void    wait_all_cmds(pid_t *pids, int cmd_count, t_shell *shell);

/*----------------------------------------
builtins
----------------------------------------*/

//builtin_echo.c
int     ft_echo(t_cmd *cmd);

// builtins/builtin_pwd.c
int     ft_pwd(void);

// builtins/builtin_env.c
int     ft_env(t_shell *shell);

// builtins/builtin_cd.c
int     ft_cd(t_cmd *cmd, t_shell *shell);

// builtins/builtin_export.c
int     ft_export(t_cmd *cmd, t_shell *shell);

// builtins/builtin_unset.c
int     ft_unset(t_cmd *cmd, t_shell *shell);

// builtins/builtin_exit.c
int     ft_exit(t_cmd *cmd, t_shell *shell);

// builtin_utils.c (ビルトイン判定)
int     is_builtin(char *cmd);
int     exec_builtin(t_cmd *cmd, t_shell *shell);


/*----------------------------------------
others
----------------------------------------*/
// debug.c
void debug_print_tokens(t_token *tokens); //! Debug提出前に削除
void debug_print_cmds(t_cmd *cmd); //! Debug提出前に削除

#endif /* MINISHELL_H */

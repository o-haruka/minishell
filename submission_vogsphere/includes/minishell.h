/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkuninag <hkuninag@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:53:47 by homura            #+#    #+#             */
/*   Updated: 2026/05/05 10:06:09 by hkuninag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include "../stdio/get_next_line/get_next_line.h"
# include "libft.h"
# include <errno.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_signal;

/***********************************
Enum & Structs
***********************************/
typedef enum e_token_kind
{
	TK_WORD,
	TK_PIPE,
	TK_REDIRECT_IN,
	TK_REDIRECT_OUT,
	TK_APPEND,
	TK_HEREDOC,
	TK_EOF
}					t_token_kind;

typedef struct s_token
{
	char			*word;
	t_token_kind	kind;
	struct s_token	*next;
}					t_token;

typedef struct s_redir
{
	t_token_kind	kind;
	char			*file;
	int				fd;
	struct s_redir	*next;
}					t_redir;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}					t_env;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_shell
{
	t_env			*env;
	t_cmd			*cmds;
	t_token			*tokens;
	int				last_status;
}					t_shell;

/************************************
Prototypes
************************************/
/*----------------------------------------
common	util(utils.c)
----------------------------------------*/
void				print_error_msg(char *cmd_name, char *target,
						char *error_msg);
int					is_directory(const char *path);

/*----------------------------------------
main (main.c)
----------------------------------------*/
void				minishell_loop(t_shell *shell);

/*----------------------------------------
signal (signal.c)
----------------------------------------*/
void				setup_signals(void);
void				set_signal_for_parent_wait(void);
void				set_signal_for_child(void);
void				set_signal_for_heredoc_child(void);

/*----------------------------------------
tokenizer
----------------------------------------*/
t_token				*tokenize(char *line);
t_token				*token_new(char *word, t_token_kind kind);
void				token_add_back(t_token **head, t_token *new_token);
void				token_free(t_token **head);
bool				is_space(char c);
bool				is_metachar(char c);
void				consume_space(char **line);
int					has_unclosed_quote(char *line);
int					ft_check_syntax(t_token *tokens);
int					append_operator(t_token **head, char **line);
int					append_word(t_token **head, char **line);

/*----------------------------------------
parser
----------------------------------------*/
t_cmd				*parse(t_token *tokens);
void				free_cmd(t_cmd *cmd);
void				free_cmds_list(t_cmd *cmd);
int					handle_redirection(t_cmd *cmd, t_token **current);
void				ft_expand_args(t_shell *shell);
char				*ft_find_env(t_env *env, char *key, int len);
int					ft_get_var_len(char *s);
char				*ft_append_char(char *result, char *str, int *i);
char				*ft_append_expanded(char *result, char *part);

/*----------------------------------------
env
----------------------------------------*/
t_env				*init_env(char **envp);
void				free_env(t_env *env);
char				*get_env_value(t_env *env, char *key);
void				env_add_back(t_env **head, t_env *new_node);
char				**env_to_envp(t_env *env);
int					update_env_value(t_env **env, char *key, char *new_value);

/*----------------------------------------
excecter
----------------------------------------*/
void				ft_execute(t_shell *shell);
char				*search_path(char *cmd, t_env *env);
int					ft_apply_redirs(t_cmd *cmd);
int					read_heredoc_input(char *delimiter);
int					prepare_heredocs(t_cmd *cmd);
void				ft_execute_pipeline(t_shell *shell);
int					count_cmds(t_cmd *cmd);
int					open_all_pipes(int (*pipes)[2], int pipe_count);
void				close_all_pipes(int (*pipes)[2], int pipe_count);
void				wait_all_cmds(pid_t *pids, int cmd_count, t_shell *shell);
void				free_envp(char **envp);
int					execve_exit_status(void);
void				update_last_status(int status, t_shell *shell);
void				do_execve(char *path, t_cmd *cmd, t_shell *shell);
void				exec_child(char *path, t_cmd *cmd, t_shell *shell);
int					wait_for_child(pid_t pid, int *status);
t_cmd				*get_nth_cmd(t_cmd *cmd, int idx);

/*----------------------------------------
builtins
----------------------------------------*/
int					ft_echo(t_cmd *cmd);
int					ft_pwd(void);
int					ft_env(t_shell *shell);
int					ft_cd(t_cmd *cmd, t_shell *shell);
int					ft_export(t_cmd *cmd, t_shell *shell);
int					ft_unset(t_cmd *cmd, t_shell *shell);
int					ft_exit(t_cmd *cmd, t_shell *shell);
int					is_builtin(char *cmd);
int					call_builtin(t_cmd *cmd, t_shell *shell);

#endif

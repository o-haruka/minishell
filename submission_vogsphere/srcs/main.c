/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:35:18 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 17:47:17 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Read one line of input via readline (interactive) or get_next_line (pipe).
** Strips the trailing newline in non-interactive mode.
*/
static char	*read_line_input(void)
{
	char	*line;

	disable_echoctl();
	if (isatty(STDIN_FILENO))
		line = readline(GREEN "minishell> " RESET);
	else
		line = get_next_line(STDIN_FILENO);
	restore_echoctl();
	if (line && !isatty(STDIN_FILENO))
		line[ft_strlen(line) - 1] = '\0';
	return (line);
}

/*
** Tokenize, parse, expand, and execute one command line.
** Called per iteration of minishell_loop.
*/
static void	process_command(t_shell *shell, char *line)
{
	if (g_signal == SIGINT)
		shell->last_status = 128 + g_signal;
	if (!*line)
		return ;
	if (*line)
		add_history(line);
	shell->tokens = tokenize(line);
	if (!shell->tokens)
	{
		shell->last_status = 2;
		return ;
	}
	shell->cmds = parse(shell->tokens);
	if (!shell->cmds)
	{
		token_free(&(shell->tokens));
		shell->last_status = 2;
		return ;
	}
	ft_expand_args(shell);
	if (shell->cmds)
		ft_execute(shell);
	free_cmds_list(shell->cmds);
	token_free(&(shell->tokens));
}

/*
** Main shell loop: read a line, process it, repeat until EOF.
*/
void	minishell_loop(t_shell *shell)
{
	char	*line;

	while (1)
	{
		g_signal = 0;
		line = read_line_input();
		if (line == NULL)
		{
			if (isatty(STDIN_FILENO))
				ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		process_command(shell, line);
		free(line);
	}
	free_env(shell->env);
	rl_clear_history();
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	shell = (t_shell){NULL, NULL, NULL, 0};
	shell.env = init_env(envp);
	if (!shell.env)
		return (1);
	setup_signals();
	minishell_loop(&shell);
	return (shell.last_status);
}

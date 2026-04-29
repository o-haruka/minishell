/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 16:30:03 by homura            #+#    #+#             */
/*   Updated: 2026/04/28 17:11:30 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>

/*-----テスト-----
minishell> env と打って、最初の PWD と OLDPWD を確認する。
minishell> cd /tmp で移動する。
minishell> env と打って、もう一度環境変数リストを見る。

PWD=/tmp となり、OLDPWDが前までいたディレクトリのパスに書き換わっていれば、環境変数の更新ロジックは成功。
*/
/*
** ==========================================
** どこへ移動するか（引数のチェック）
** ==========================================
** 成功時は移動先のパス(文字列)を返し、失敗時はNULLを返すヘルパー関数
*/
static char	*get_cd_path(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->args[1] == NULL)
	{
		path = get_env_value(shell->env, "HOME");
		if (path == NULL)
			print_error_msg("cd", NULL, "HOME not set");
		return (path);
	}
	else if (cmd->args[2] != NULL)
	{
		print_error_msg("cd", NULL, "too many arguments");
		return (NULL);
	}
	return (cmd->args[1]);
}

/*
** ==========================================
** 環境変数の更新
** ==========================================
** PWD と OLDPWD を新しいパスで上書きするヘルパー関数
*/
static int	update_pwd_error(char *old_pwd, char *new_pwd)
{
	print_error_msg(NULL, "malloc", strerror(errno));
	free(old_pwd);
	free(new_pwd);
	return (1);
}

static int	update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (old_pwd)
	{
		if (update_env_value(&(shell->env), "OLDPWD", old_pwd) != 0)
			return (update_pwd_error(old_pwd, new_pwd));
	}
	if (new_pwd)
	{
		if (update_env_value(&(shell->env), "PWD", new_pwd) != 0)
			return (update_pwd_error(old_pwd, new_pwd));
	}
	free(old_pwd);
	free(new_pwd);
	return (0);
}

/*
** ==========================================
** 実際に移動する (chdir) ＆ 全体の統括
** ==========================================
*/
int	ft_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	*old_pwd;

	path = get_cd_path(cmd, shell);
	if (path == NULL)
		return (1);
	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
	{
		free(old_pwd);
	}
	if (chdir(path) != 0)
	{
		print_error_msg("cd", path, strerror(errno));
		free(old_pwd);
		return (1);
	}
	return (update_pwd_vars(shell, old_pwd));
}

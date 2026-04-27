#include "minishell.h"
#include <string.h> // strerrorを使うために必要
#include <errno.h>  // errnoを使うために必要

/*
** 汎用エラー出力関数（ビルトイン・外部コマンド両対応）
** cmd_name が NULL でなければ "cmd_name: " を追加出力する
*/
void	print_error_msg(char *cmd_name, char *target, char *error_msg)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (cmd_name != NULL)
	{
		ft_putstr_fd(cmd_name, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (target != NULL)
	{
		ft_putstr_fd(target, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putendl_fd(error_msg, STDERR_FILENO);
}
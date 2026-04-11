#include "minishell.h"
#include <stdlib.h>

// t_redirリストを解放
static void free_redirs(t_redir *redir)
{
    t_redir *tmp;
    while (redir)
    {
        tmp = redir->next;
        if (redir->file)
            free(redir->file);
        free(redir);
        redir = tmp;
    }
}

// t_cmd構造体を解放
// ・args配列の各要素（cmd->args[0]～cmd->args[i-1]）
// ・args配列本体
// ・redirsリスト
// ・cmd本体
// をすべてfreeしてくれる関数。
void free_cmd(t_cmd *cmd)
{
    int i;
    if (!cmd)
        return;
    if (cmd->args)
    {
        i = 0;
        while (cmd->args[i])
        {
            free(cmd->args[i]);
            i++;
        }
        free(cmd->args);
    }
    free_redirs(cmd->redirs);
    free(cmd);
}

// t_cmdリスト全体を解放
void free_cmds_list(t_cmd *cmd)
{
    t_cmd *tmp;
    while (cmd)
    {
        tmp = cmd->next;
        free_cmd(cmd);
        cmd = tmp;
    }
}

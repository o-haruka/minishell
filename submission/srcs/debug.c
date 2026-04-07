
#include "minishell.h"
#include <stdio.h>

// パース結果（t_cmdのリスト）をターミナルに表示するデバッグ関数
void debug_print_cmds(t_cmd *cmd)
{
    int cmd_idx = 0;
    printf(RED "\n☆ ★ ☆ ★ ☆ ★ ☆ ★[Debug: Parser]☆ ★ ☆ ★ ☆ ★ ☆ ★\n" RESET);
    printf("\n=== 📦 Parser Output ===\n");
    while (cmd)
    {
        printf(GREEN"[Command %d]\n"RESET, cmd_idx++);
        // 引数(args)の表示
        printf("  args:\n");
        for (int i = 0; cmd->args && cmd->args[i] != NULL; i++)
        {
            printf("    [%d]: '%s'\n", i, cmd->args[i]);
        }
        // リダイレクト(redirs)の表示
        printf("  redirs:\n");
        t_redir *r = cmd->redirs;
        if (!r) printf("    (none)\n");
        while (r)
        {
            char *kind_str;
            if (r->kind == TK_REDIRECT_IN) kind_str = "<";
            else if (r->kind == TK_REDIRECT_OUT) kind_str = ">";
            else if (r->kind == TK_APPEND) kind_str = ">>";
            else if (r->kind == TK_HEREDOC) kind_str = "<<";
            else kind_str = "?";
            printf("    kind: %s, file: '%s'\n", kind_str, r->file);
            r = r->next;
        }
        cmd = cmd->next;
        if (cmd) printf(PINK"\n    ↓ (PIPE)\n\n"RESET);
    }
    printf("========================\n\n");
    printf(RED "☆ ★ ☆ ★ ☆ ★ ☆ ★[Debug End]☆ ★ ☆ ★ ☆ ★ ☆ ★\n" RESET);
}

// [Debug] トークンの中身を見てみる (開発中はこれをコメントアウトして確認)
void debug_print_tokens(t_token *tokens) {
    t_token *curr = tokens;
    t_token *tmp = curr;
    printf(RED "\n☆ ★ ☆ ★ ☆ ★ ☆ ★[Debug: Token]☆ ★ ☆ ★ ☆ ★ ☆ ★\n" RESET);
    while (tmp)
    {
        if (tmp->word)
            printf(GREEN"[%s]"RESET, tmp->word);
        else
            printf("[(null)]");
        if (tmp->next)
            printf("->");
        tmp = tmp->next;
    }
    printf("\n");
    // 各トークンの詳細
    while (curr) {
        printf("Token: kind=%d, word=[%s]\n", curr->kind, curr->word ? curr->word : "(null)");
        curr = curr->next;
    }
    printf(RED "☆ ★ ☆ ★ ☆ ★ ☆ ★[Debug End]☆ ★ ☆ ★ ☆ ★ ☆ ★\n" RESET);
}

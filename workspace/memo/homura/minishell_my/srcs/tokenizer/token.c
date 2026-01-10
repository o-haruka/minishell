#include "minishell.h"

// 新しいトークンを作成する
t_token *token_new(char *word, t_token_kind kind)
{
    t_token *token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    token->word = word;
    token->kind = kind;
    token->next = NULL;
    return (token);
}

// リストの末尾に追加する
void token_add_back(t_token **head, t_token *new_token)
{
    t_token *last;

    if (!head || !new_token)
        return;
    if (*head == NULL) // リストが空なら、これが先頭になる
    {
        *head = new_token;
        return;
    }
    last = *head;
    while (last->next != NULL) // 最後のノードまで移動
        last = last->next;
    last->next = new_token; // 連結！
}

// トークンリスト全体を解放する
void token_free(t_token **head)
{
    t_token *current;
    t_token *next;

    if (!head || !*head)
        return;

    current = *head;
    while (current)
    {
        next = current->next;
        // トークンが持つ文字列（ft_substrで作成されたもの）を解放
        if (current->word)
            free(current->word);
        // トークン構造体自体を解放
        free(current);
        current = next;
    }
    *head = NULL;
}

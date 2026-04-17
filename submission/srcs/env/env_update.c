#include "minishell.h"
#include "libft.h"

/*------------------------------------------------------------------
** t_env リストから key を探し、見つかったらその value を new_value で上書きする関数
------------------------------------------------------------------*/

/* ** 1. リストの末尾に追加する汎用関数（後で export でも使い回せます！）
*/
void env_add_back(t_env **head, t_env *new_node)
{
    t_env *current;

    if (!head || !new_node)
        return ;
    if (*head == NULL) // c. リストの末尾に繋ぐ
        *head = new_node;  // これが最初のノードになる
    else
    {
        current = *head;
        while (current->next != NULL) // 最後尾を探す
            current = current->next;
        current->next = new_node; // 末尾に繋ぐ
    }
}

/* ** 2. 新規ノードを作成して追加する関数
*/
static int add_new_env_node(t_env **env_head, char *key, char *value)
{
    t_env *new_node;

    // a. 新しいノードの作成
    new_node = malloc(sizeof(t_env));
    if (!new_node)
        return (1);

    // b. key と value を複製してセット
    new_node->key = ft_strdup(key);
    new_node->value = NULL;
    if (value)
        new_node->value = ft_strdup(value);
    new_node->next = NULL;
    // どちらかの malloc が失敗した場合のエラー処理
    if (!new_node->key || (value && !new_node->value))
    {
        free(new_node->key);
        free(new_node->value); // free(NULL)は安全なのでif不要
        free(new_node);
        return (1);
    }
    env_add_back(env_head, new_node);
    return (0);
}

/* ** 3. メインの更新関数
*/
int update_env_value(t_env **env_head, char *key, char *new_value)
{
    t_env *current;

    if (!env_head || !key)
        return (1);
    current = *env_head;
    while (current)
    {
        if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
        {
            // 1. 古い値を free してメモリリークを防ぐ
            free(current->value); // 古い値を解放 (NULLでも安全)
            current->value = NULL;

            // 2. 新しい値を strdup で複製して代入する
            if (new_value)
            {
                current->value = ft_strdup(new_value);
                if (!current->value)
                    return (1); // malloc失敗
            }
            else
                current->value = NULL;

            return (0); // 上書き成功
        }
        current = current->next;
    }
    // ループを抜けた＝見つからなかったので、追加関数に丸投げ！
    return (add_new_env_node(env_head, key, new_value));
}
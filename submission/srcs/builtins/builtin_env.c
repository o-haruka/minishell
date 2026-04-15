#include "minishell.h"
#include "libft.h"

int ft_env(t_shell *shell)
{
    t_env *current;

    current = shell->env;
    while (current)
    {
        // envコマンドは、値（value）が存在するものだけを出力するのが本家の仕様。
        // 本物のBashでenvコマンドの仕様
        //      ・ 値があるもの→表示
        //      ・ 値がないキーだけの変数→内部には存在しているけれどenvの出力からは隠される
        // 条件分岐を入れておくことで本物のBashと全く同じ挙動を再現できる。
        if (current->value != NULL)
        {
            ft_putstr_fd(current->key, STDOUT_FILENO);
            ft_putchar_fd('=', STDOUT_FILENO);
            ft_putendl_fd(current->value, STDOUT_FILENO);
        }
        current = current->next;
    }
    return (0); // 成功ステータス
}

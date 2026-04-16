#include "minishell.h"
#include "libft.h"
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
static char *get_cd_path(t_cmd *cmd, t_shell *shell)
{
    char *path;

    if (cmd->args[1] == NULL)
    {
        // 引数なし (単なる `cd`) の場合、HOMEディレクトリを探す
        path = get_env_value(shell->env, "HOME");
        // ?NULLになるときってどんなとき？
        // case1: unset HOMEしたときpath == NULLになるのでエラー処理が必要。
        // case2: $ env -i ./minishell (環境変数をすべて空っぽにして起動)
        if (path == NULL)
            ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
        return (path);
    }
    else if (cmd->args[2] != NULL)
    {
        // 本家の仕様：引数が2つ以上あるとエラーになる
        ft_putendl_fd("minishell: cd: too many arguments", STDERR_FILENO);
        return (NULL);
    }
    // 引数が1つの場合 (例: `cd /tmp`) は、その引数をパスにする
    return (cmd->args[1]);
}

/*
** ==========================================
** 環境変数の更新
** ==========================================
** PWD と OLDPWD を新しいパスで上書きするヘルパー関数
*/
static int update_pwd_vars(t_shell *shell, char *old_pwd)
{
    char *new_pwd;

    // 1. 移動が成功したので、新しい現在地を取得する
    new_pwd = getcwd(NULL, 0);

    // 2. リストの OLDPWD を、先ほど確保しておいた old_pwd で上書き
    if (old_pwd)
    {
        if (update_env_value(&(shell->env), "OLDPWD", old_pwd) != 0)
        {
            perror("minishell: malloc failed");
            free(old_pwd);
            free(new_pwd);
            return (1);
        }
    }

    // 3. リストの PWD を、いま取得した new_pwd で上書き
    if (new_pwd)
    {
        if (update_env_value(&(shell->env), "PWD", new_pwd) != 0)
        {
            perror("minishell: malloc failed");
            free(old_pwd);
            free(new_pwd);
            return (1);
        }
    }

    // 4. getcwd で自動 malloc された文字列を解放する
    // （リストの中には ft_strdup でコピーされたものが入っているので大元は消してOK）
    free(old_pwd);
    free(new_pwd);
    return (0);
}

/*
** ==========================================
** 実際に移動する (chdir) ＆ 全体の統括
** ==========================================
*/
int ft_cd(t_cmd *cmd, t_shell *shell)
{
    char *path;
    char *old_pwd;

    // 1. どこへ移動するか取得（NULLならエラーなので即終了）
    path = get_cd_path(cmd, shell);
    if (path == NULL)
        return (1);

    // 2. 準備編：移動する「前」の現在地を確保しておく
    old_pwd = getcwd(NULL, 0);
    if(!old_pwd){
        //TODO ここにエラー処理
        // ft_putendl_fd(old_pwd, STDOUT_FILENO);
        free(old_pwd);
    }

    // 3. chdir・・・自プロセスのカレントディレクトリを引数のpathに変更する
    // 成功すると 0、失敗すると -1 を返す
    if (chdir(path) != 0)
    {
        // 失敗した場合（例: ディレクトリがない、権限がない）
        perror("minishell: cd");
        free(old_pwd); // 失敗した時は old_pwd を忘れずに free して終わる
        return (1);
    }

    // 4. 移動に成功したら、環境変数の更新処理へ丸投げ
    return (update_pwd_vars(shell, old_pwd));
}
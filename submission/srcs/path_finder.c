#include "minishell.h"
#include "libft.h"
#include <unistd.h>


/*
get_env_path() → PATH環境変数を取得
ft_split() → パス文字列を:で分割
search_path() → 各ディレクトリでコマンドを探索
access() → 実行権限を確認
minishellにおいて、ユーザーが入力したコマンド（例：ls、grepなど）の実行可能ファイルを見つけるための最初のステップとして機能しています。
*/


/*
** 環境変数配列 envp から "PATH" で始まる変数の値を探す
*/
char *get_env_path(char **envp){
    int i;
    i = 0;
    while(envp[i]){
        // PATH環境変数を探す
        if(ft_strncmp(envp[i], "PATH=", 5) == 0){
            return (envp[i] + 5); // "PATH="の部分をスキップして返す
        }
        i++;
    }
    // 見つからなかった場合はNULLを返す
    return NULL;
}

char *join_path(char *dir, char *cmd){
    char *tmp;
    char *full_path;

    // ディレクトリとコマンド名を結合してフルパスを作成
    tmp = ft_strjoin(dir, "/");
    if(tmp == NULL)
        return (NULL);
    full_path = ft_strjoin(tmp, cmd);
    free(tmp);// ft_strjoinで確保したメモリを解放
    return (full_path);
}

/*
** コマンドのフルパスを検索して返す
** 見つからない場合は NULL を返す
*/
char *search_path(char *cmd, char **envp){
    char **paths;
    char *path_env;
    char *full_path;
    int i;

    path_env = get_env_path(envp);
    if(path_env == NULL)
        return (NULL);

    // PATH環境変数の値を ':' で分割してディレクトリの配列を作成
    paths = ft_split(path_env, ':');
    if(paths == NULL)
        return (NULL);

    i = 0;
    while(paths[i]){
        full_path = join_path(paths[i], cmd);
        // access(path, X_OK) で「実行権限のあるファイルが存在するか」確認
        if(full_path && access(full_path, X_OK) == 0){
            // 実行可能なファイルが見つかった場合、そのパスを返す
            free_array(paths);
            return (full_path);
        }
        free(full_path);
        i++;
    }
    free_array(paths);
    return (NULL);
}
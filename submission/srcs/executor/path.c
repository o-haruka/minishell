#include "minishell.h"


/*--------------------------------------------
** PATH探索の概要
** - PATHの値はenv_utils.cのget_env_value関数から取得
** - ft_split()でパス文字列を:で分割
** - search_path()で各ディレクトリを探索
** - access()で実行権限を確認
**
** minishellでユーザーが入力したコマンド（例: ls, grepなど）の
** 実行ファイルを見つけるための最初のステップ。
---------------------------------------------*/

/*--------------------------------------------
** 1. 文字列配列の解放
** 二重ポインタで確保した配列を全てfreeする
---------------------------------------------*/
static void free_array(char **arr)
{
	int i;
	i = 0;
	if (!arr)
		return;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
/*--------------------------------------------
** 2. ディレクトリパスとコマンド名を '/' で結合する
** 例: dir="/bin", cmd="ls" -> "/bin/ls"
** /bin
** /bin/ (末尾の`/`を足す)
** /bin/ls (lsを足す)
** `PATH変数で見つかったディレクトリ` + `/` + `コマンド名`
---------------------------------------------*/
static char	*join_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full_path;

	// ディレクトリとコマンド名を結合してフルパスを作成
	// TODO: ft_strjoinはmalloc使用なのでfree確認　+ errorチェック
	tmp = ft_strjoin(dir, "/");
	if (tmp == NULL)
		return (NULL);
	// TODO: ft_strjoinはmalloc使用なのでfree確認　+ errorチェック
	full_path = ft_strjoin(tmp, cmd);
	free(tmp); // ft_strjoinで確保したメモリを解放
	return (full_path);
}

/*--------------------------------------------
** 3. PATH配列から実行可能ファイルを探索
** PATHの配列を巡回し、実行可能なフルパスを探す
** 見つかればそのフルパスを、見つからなければNULLを返す
---------------------------------------------*/
static char *find_executable_in_paths(char **paths, char *cmd)
{
    char    *full_path;
    int     i;

    i = 0;
    while (paths[i])
    {
        full_path = join_path(paths[i], cmd);
        if (full_path == NULL)
            return (NULL);
        if (access(full_path, X_OK) == 0)
            return (full_path);
        free(full_path);
        i++;
    }
    return (NULL);
}

/*--------------------------------------------
** 4. コマンドのフルパス検索
** - コマンドに'/'が含まれていれば直接パスを確認
** - 含まれていなければPATH環境変数からディレクトリ配列を作成し探索
** - 実行可能なパスが見つかればそのフルパス、なければNULLを返す
---------------------------------------------*/
char *search_path(char *cmd, t_env *env)
{
	char **paths;
	char *path_env;
	char *exec_path;

	// 1. コマンドに '/' が含まれているかチェック (相対パス・絶対パスの直指定)
	// 例: "./minishell", "/bin/ls" などの場合はPATHを探さずに直接チェックする
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0) // F_OK: 存在確認
			return (ft_strdup(cmd));
		return (NULL);
	}
	// 2. PATH環境変数の値を取得
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	// 3. PATHを':'で分割しディレクトリ配列を作成
	paths = ft_split(path_env, ':');
	if (paths == NULL)
		return (NULL);
	// ① ここで関数を呼び出し、結果（フルパス or NULL）を受け取る
	exec_path = find_executable_in_paths(paths, cmd);
	// ② 結果がどちらであっても、使い終わった paths 配列をここで解放する
	free_array(paths);
	// ③ 結果（フルパス or NULL）を呼び出し元に返す
	return (exec_path);
}

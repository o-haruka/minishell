#include "libft.h"
#include "minishell.h"

/*
PATHの値はenv_utils.cのget_env_value関数から取得。
ft_split() → パス文字列を:で分割
search_path() → 各ディレクトリでコマンドを探索
access() → 実行権限を確認
minishellにおいて、ユーザーが入力したコマンド（例：ls、grepなど）の実行可能ファイルを見つけるための最初のステップとして機能する。
*/

static void free_array(char **arr)
{
	int i = 0;
	if (!arr)
		return;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/*
** ディレクトリパスとコマンド名を '/' で結合する
** 例: dir="/bin", cmd="ls" -> "/bin/ls"
** /bin
** /bin/ (末尾の`/`を足す)
** /bin/ls (lsを足す)
** `PATH変数で見つかったディレクトリ` + `/` + `コマンド名`
*/
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

/*
** コマンドのフルパスを検索して返す
** 見つからない場合は NULL を返す
*/
char	*search_path(char *cmd, t_env *env)
{
	char **paths;
	char *path_env;
	char *full_path;
	int i;

	// 1. コマンドに '/' が含まれているかチェック (相対パス・絶対パスの直指定)
	// 例: "./minishell", "/bin/ls" などの場合はPATHを探さずに直接チェックする
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK | X_OK) == 0) // F_OK: 存在確認, X_OK: 実行権限確認
			return (ft_strdup(cmd));
		return (NULL);
	}

	// 2. 環境変数リストから "PATH" の値を取得する
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);

	// PATH環境変数の値を ':' で分割してディレクトリの配列を作成
    // TODO: ft_strjoinはmalloc使用なのでfree確認　+ errorチェック
	paths = ft_split(path_env, ':');
	if (paths == NULL)
		return (NULL);

	i = 0;
	while (paths[i])
	{
		full_path = join_path(paths[i], cmd);
		if (full_path == NULL)
		{
			free_array(paths);
			return (NULL);
		}
		// access(path, X_OK) で「実行権限のあるファイルが存在するか」確認
		if (access(full_path, X_OK) == 0)
		{
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
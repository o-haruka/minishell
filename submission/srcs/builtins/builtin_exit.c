/*hkuninag担当*/

#include "minishell.h"
#include "libft.h"

/*
** 動的確保したメモリを解放してから exit() を呼ぶ。
** code & 0xFF で終了コードを 0〜255 に収める。
** (例: 256 → 0、257 → 1、-1 → 255)
*/
static void	ft_do_exit(t_shell *shell, int code)
{
	// 環境変数リスト、コマンドリスト、トークンリストを解放
	free_env(shell->env);
	free_cmds_list(shell->cmds);
	token_free(&shell->tokens);
	/*
	** exit() に渡せる終了コードは 0〜255 の範囲のみ。
	** & 0xFF は下位8ビットだけを残すビット演算で、
	** 範囲外の値を自動的に 0〜255 に丸める。
	*/
	exit(code & 0xFF); // 終了コードを0〜255に丸めて終了
}

/*
** exit ビルトインの実装。
** 引数なし        → last_status の値で終了
** 引数1つ(数値)   → その値で終了
** 引数1つ(非数値) → エラーを表示して終了しない
** 引数2つ以上     → エラーを表示して終了しない
*/
int	ft_exit(t_cmd *cmd, t_shell *shell)
{
	// bash と同様に、exit コマンドが実行されたことを表示する
	ft_putendl_fd("exit", STDOUT_FILENO);
	if (!cmd->args[1]) // 引数なし → last_status の値で終了
		ft_do_exit(shell, shell->last_status);
	if (cmd->args[2]) // args[2] が存在する = 引数が2つ以上
	{
		// 引数が多すぎる場合はエラーを出してシェルを続ける
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		shell->last_status = 1; // エラーなので 1 をセット
		return (1); // exit() せずに return → シェルは続く
	}
	if (!ft_isint(cmd->args[1])) // ft_isint が 0 = 数値でない
	{
		// 数値でない引数の場合はエラーを出してシェルを続ける
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[1], STDERR_FILENO); // 問題の引数を表示
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		shell->last_status = 255; // bash の仕様： 数値でない場合は 255 をセット
		return (255); // exit() せずに return → シェルは続く
	}
	// 数値引数を int に変換して終了
	ft_do_exit(shell, ft_atoi(cmd->args[1]));
	return (0); // ここには到達しないがコンパイラ警告を防ぐ
}

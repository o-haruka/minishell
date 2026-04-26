#include "minishell.h"
#include <sys/wait.h>

/*--------------------------------------------
** 1. 子プロセスの処理 (pid == 0)
** execve実行の前に fd を付け替える。
** (execve で起動するコマンドは fd 0・1 番だけを使うため)
** execve 後は fd を変更できないため、先に付け替える。
** 最新の環境変数リストから配列を生成し、プログラムを生まれ変わらせる。
--------------------------------------------*/
static void exec_child(char *path, t_cmd *cmd, t_shell *shell)
{
    char    **current_envp;

    if (ft_apply_redirs(cmd) == -1)
    {
        free(path);
        exit(1);
    }
    current_envp = env_to_envp(shell->env);
    if (!current_envp)
        exit((free(path), 1)); // env_to_envp 失敗。path を解放してから exit
    execve(path, cmd->args, current_envp); // 成功するとこのプロセスがコマンドに置き換わり、以降の行は実行されない
    perror("minishell: execve");
    free_envp(current_envp); // execve 失敗時のみここに到達。配列と各文字列を解放
    free(path);
    exit(1);
}

/*--------------------------------------------
** 2. 親プロセスの処理 (pid > 0)
** 親プロセスは子プロセスが終了するのを待つ。
** WIFEXITED等で正常・異常終了を確認し、ステータスを取り出す。
** シグナルで強制終了した場合、bashの仕様に合わせて
** "Quit: 3" や "^C のあとの改行" の見た目処理を行う。
** 待機が終わったので親プロセスのシグナルを元の設定に戻す。
--------------------------------------------*/
static void exec_parent(pid_t pid, t_shell *shell, char *path)
{
    int status;

    if (waitpid(pid, &status, 0) == -1)
    {
        perror("minishell: waitpid");
        free(path);
        return ;
    }
    if (WIFEXITED(status))
        shell->last_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
    {
        shell->last_status = 128 + WTERMSIG(status);
        if (WTERMSIG(status) == SIGQUIT)
            ft_putendl_fd("Quit: 3", STDERR_FILENO);
        else if (WTERMSIG(status) == SIGINT)
            write(STDERR_FILENO, "\n", 1);
    }
    setup_signals();
    free(path);
}

/*--------------------------------------------
** 3. プロセスを分岐（fork）させて実行する関数
** pid == 0: 子プロセスの処理（OSのデフォルトシグナルをセット）
** pid >  0: 親プロセスの処理（シグナル飛んできても無視）
--------------------------------------------*/
static void run_process(char *path, t_cmd *cmd, t_shell *shell)
{
    pid_t   pid;

    pid = fork();
    if (pid < 0)
    {
        perror("minishell: fork");
        free(path);
        return ;
    }
    if (pid == 0)
    {
        set_signal_for_child();
        exec_child(path, cmd, shell);
    }
    else
    {
        set_signal_for_parent_wait();
        exec_parent(pid, shell, path);
    }
}

/*--------------------------------------------
** 4. 外部コマンド（ビルトインじゃない）の実行処理
** 実行ファイルの絶対パスを探す。
** パスが見つからなかった場合はエラーを出力して終了ステータスを127に。
--------------------------------------------*/
static void execute_external(t_cmd *cmd, t_shell *shell)
{
    char    *path;

    path = search_path(cmd->args[0], shell->env);
    if (!path)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(cmd->args[0], STDERR_FILENO);
        ft_putendl_fd(": command not found", STDERR_FILENO);
        shell->last_status = 127;
        return ;
    }
    run_process(path, cmd, shell);
}

/*--------------------------------------------
** 5. 単一コマンドを実行する関数（ディスパッチャ）
** コマンド名が空でないか確認し、以下の順に振り分ける。
** 1. パイプがある場合は ft_execute_pipeline に丸投げ
** 2. ビルトインコマンドの場合は exec_builtin を呼んで帰る
** 3. それ以外は外部コマンドとして execute_external を呼ぶ
--------------------------------------------*/
void    ft_execute(t_shell *shell)
{
    t_cmd   *cmd;

    cmd = shell->cmds;
    if (!cmd || !cmd->args || !cmd->args[0])
        return ;
    if (cmd->next != NULL)
    {
        ft_execute_pipeline(shell);
        return ;
    }
    if (is_builtin(cmd->args[0]))
    {
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);
        if (ft_apply_redirs(cmd) == -1)
            return;
        shell->last_status = exec_builtin(cmd, shell);
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdout);
        close(saved_stdin);
        return ;
    }
    execute_external(cmd, shell);
}

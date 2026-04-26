#include "minishell.h"

/* ーーーーーーーーーーーーーーーーーーーー
アプローチA
ーーーーーーーーーーーーーーーーーーーー*/
//! バッファ固定でpathが入り切らなかったらどうなるの？？
// 1024とは・・・The function getwd() is a compatibility routine which calls getcwd() with its buf argument and a size of MAXPATHLEN (as defined in the include file <sys/param.h>).  Obviously, buf should be at least MAXPATHLEN bytes in length.

// TODO realloc作成。
// 文字列を保存するためのメモリ（配列）をスタック領域に確保しています。MacやLinuxでは、パスの最大長は PATH_MAX（通常1024バイト）と定義されていることが多いので、1024を用意しておけば基本的には安全。
// → 1024よりオーバーした時の挙動ってどうなっているのか？？

// ?getcwd(buf, sizeof(buf))
// OSに対して「今の現在地を調べて、buf に 1024 文字以内で書き込んでね」とお願いします。
// ・成功時: buf の中に "/Users/harukaomura/..." という文字列が入り、getcwd は buf を返します（NULL ではない）。
// ・失敗時: 例えば「現在いるディレクトリが、別のターミナルから削除されてしまって存在しない」などの異常事態が起きると、getcwd は NULL を返します。

// ?return (0); と return (1);
// シェルにおいて、コマンドが正常終了した時は 0、エラーが起きた時は 1 以上の数値を返すのが世界共通のルール。

// int ft_pwd(void)
// {
//     char buf[1024]; // パスを保存するバッファ。PATH_MAXが使えない環境もあるので、一旦1024で十分です。

//     // getcwdは現在地をbufに書き込みます。失敗するとNULLを返します。
//     if (getcwd(buf, sizeof(buf)) != NULL)
//     {
//         ft_putendl_fd(buf, STDOUT_FILENO); // 標準出力に書き込んで改行
//         return (0); // 成功ステータス
//     }
//     else
//     {
//         perror("minishell: pwd");
//         return (1); // 失敗ステータス
//     }
// }

/* ーーーーーーーーーーーーーーーーーーーー
アプローチB
ーーーーーーーーーーーーーーーーーーーー*/
//?Macや最近のLinuxの getcwd には、「裏技（拡張仕様）」が用意されている。
//第1引数に NULL、第2引数に 0 を渡すと、getcwd 自身が必要な文字数を計算し、勝手にmallocでピッタリの箱を作ってパスを入れ、そのポインタを返してくれます。

// ?man 3  getpwd
// If buf is NULL, space is allocated as necessary to store the pathname and size is ignored.  This space may later be free(3)'d.

int ft_pwd(void)
{
    char *path;

    // getcwdに全てお任せ（自動でmallocされる）
    path = getcwd(NULL, 0);
    if (path != NULL)
    {
        // ft_putendl_fd("✨This is my built-in pwd!✨", STDOUT_FILENO); //!test

        ft_putendl_fd(path, STDOUT_FILENO);
        free(path); // ★自動でmallocされているので、使い終わったら絶対にfreeする！
        return (0);
    }
    else
    {
        perror("minishell: pwd");
        return (1);
    }
}

/* ーーーーーーーーーーーーーーーーーーーー
アプローチC (自力で箱を大きくしていく（ガチのPOSIX仕様）)
ーーーーーーーーーーーーーーーーーーーー*/
//「自動mallocは使っちゃダメな場合や、古いOSでも絶対に動くようにする場合、「入らなかったら箱を2倍にする（realloc）」ループを書く。

// #include <errno.h> // ERANGE を使うために必要

// int ft_pwd(void)
// {
//     size_t  size = 1024;
//     char    *buf;

//     buf = malloc(size);
//     if (!buf)
//         return (1); // malloc失敗

//     // getcwdがNULLを返し、かつエラー理由が「箱が小さい(ERANGE)」である限りループ
//     while (getcwd(buf, size) == NULL)
//     {
//         if (errno == ERANGE)
//         {
//             size *= 2; // 箱のサイズを2倍にする
//             free(buf); // 古い箱を捨てる
//             buf = malloc(size); // 新しい大きな箱を作る
//             if (!buf)
//                 return (1);
//         }
//         else
//         {
//             // 「箱が小さい」以外のエラー（ディレクトリが消された等）
//             perror("minishell: pwd");
//             free(buf);
//             return (1);
//         }
//     }

//     // 無事に取得できたら出力
//     ft_putendl_fd(buf, STDOUT_FILENO);
//     free(buf); // 使い終わったら解放
//     return (0);
// }
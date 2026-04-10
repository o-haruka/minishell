#include "minishell.h"
#include "libft.h"

// 演算子トークンを作成し、リストに追加し、入力読み込み位置を進める
void append_operator(t_token **head, char **line)
{
    t_token_kind kind;
    char *operator_str;//!わかりにくいのでoperator_strがいいかも
    operator_str = NULL;

    // 1. 種類と文字列の決定
    if (**line == '|')
    {
        kind = TK_PIPE;
        operator_str = ft_strdup("|");
        if (!operator_str)
            return;
        (*line)++; // 1文字進める
    }
    else if (**line == '>')
    {
        // 次の文字を「先読み」
        if ((*line)[1] == '>')//「現在の場所から見て何番目か？」を表している
        {
            kind = TK_APPEND;
            operator_str = ft_strdup(">>");
            if (!operator_str)
                return;
            (*line) += 2; // 2文字進める
        }
        else
        {
            kind = TK_REDIRECT_OUT;
            operator_str = ft_strdup(">");
            if (!operator_str)
                return;
            (*line)++; // 1文字進める
        }
    }
    else if (**line == '<')
    {
        // 次の文字を「先読み」
        if ((*line)[1] == '<')
        {
            kind = TK_HEREDOC;
            operator_str = ft_strdup("<<");
            if (!operator_str)
                return;
            (*line) += 2; // 2文字進める
        }
        else
        {
            kind = TK_REDIRECT_IN;
            operator_str = ft_strdup("<");
            if (!operator_str)
                return;
            (*line)++; // 1文字進める
        }
    }
    else
    {
        // 不明な演算子（エラー処理を追加することも可能）
        return;
    }

    // 2. 新しいノードを作ってリストに繋ぐ
    t_token *new_token = token_new(operator_str, kind);
    if (!new_token)
    {
        free(operator_str); // token_new失敗時にoperator_strがリークしないように
        return;
    }
    token_add_back(head, new_token);
}

// 単語の終了位置までポインタを進め、トークンを作成する
void append_word(t_token **head, char **line)
{
    char *start;
    char *word_str;
    char quote;

    start = *line; // 1. 開始位置をメモ
    quote = 0;     // 0は「クォートに入っていない」状態

    //TODO: 閉じクォートがないときの処理がないのでは？？
    while (**line)
    {
        // A. クォート中の処理
        if (quote)
        {
            if (**line == quote) // 閉じクォートが見つかったら
                quote = 0;       // 通常モードに戻る
        }
        // B. 通常モードの処理
        else
        {
            // スペースかメタ文字が来たら、単語の終わり！
            if (is_space(**line) || is_metachar(**line))
                break;

            // クォートの始まりを見つけたら、クォートモードへ
            if (**line == '\'' || **line == '\"')
                quote = **line; // ' か " を記録する
        }
        (*line)++; // 次の文字へ
    }

    // 2. 切り出し (start から 現在の *line まで)
    // ※ ft_substr(文字列, 開始インデックス, 長さ)
    //文字列の一部を切り出して新しい文字列を作成する関数（substring = 部分文字列）。
    // ここではポインタの引き算で長さを出しています
    //!ft_substrはmalloc使用。どこでfreeする？ 
    word_str = ft_substr(start, 0, *line - start);
    if(!word_str)
        return;

    // 3. リストに追加
    t_token *new_token = token_new(word_str, TK_WORD);
    if (!new_token) {
        free(word_str);
        return;
    }
    token_add_back(head, new_token);
}

#include "minishell.h"
#include "libft.h"

/*
** t_env リストを走査し、key が完全一致する value を返す。
** 見つからなければ NULL を返す。
**
** ft_strncmp だけでなく ft_strlen でも比較する理由:
**   "HOME" で検索するとき "HOMEPATH" の先頭4文字も一致してしまうため、
**   キーの長さが完全に一致するものだけを返す。
**
** 例）key="HOME", len=4 のとき
**   env->key="HOME"     → 一致（value を返す）
**   env->key="HOMEPATH" → 不一致（スキップ）
*/
char	*ft_find_env(t_env *env, char *key, int len)
{
	while (env)
	{
		if (ft_strncmp(env->key, key, len) == 0
			&& (int)ft_strlen(env->key) == len)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** '$' の直後から変数名として有効な文字（英数字・_）が何文字続くかを返す。
**
** 例）"HOME_DIR/doc" を渡すと 8 を返す（"HOME_DIR" が変数名部分）
**    "?"           を渡すと 0 を返す（英数字・_ 以外なので変数名ではない）
*/
int	ft_get_var_len(char *s)
{
	int	len;

	len = 0;
	while (s[len] && (ft_isalnum(s[len]) || s[len] == '_'))
		len++;
	return (len);
}

/*
** str[*i] の1文字を result の末尾に連結した新しい文字列を返す。
** 処理後に *i を1つ進める。
**
** ft_strjoin は新しい文字列を確保するため、連結前の result と tmp は
** 不要になる。メモリリークを防ぐため両方を free する。
*/
char	*ft_append_char(char *result, char *str, int *i)
{
	char	*tmp;
	char	*joined;

	tmp = ft_substr(str, *i, 1); // str[*i] の1文字だけを切り出す
	if (!tmp)
	{
		free(result);
		return (NULL);
	}
	joined = ft_strjoin(result, tmp);
	free(result);
	free(tmp);
	*i += 1;
	return (joined);
}

/*
** 展開結果 part を result の末尾に連結した新しい文字列を返す。
**
** ft_strjoin は新しい文字列を確保するため、連結前の result と part は
** 不要になる。メモリリークを防ぐため両方を free する。
*/
char	*ft_append_expanded(char *result, char *part)
{
	char	*joined;

	joined = ft_strjoin(result, part);
	free(result);
	free(part);
	return (joined);
}

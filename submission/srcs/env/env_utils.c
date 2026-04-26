#include "minishell.h"

/*
** t_env リスト全体を解放する。
** key・value・ノード本体の順で free する。
*/
void	free_env(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env->next; // 次のノードを先に保存してから free
		free(env->key);
		free(env->value);
		free(env);
		env = tmp;
	}
}

/*
** t_env リストから key が一致するノードを探し、value を返す。
** 見つからなければ NULL を返す。
*/
char	*get_env_value(t_env *env, char *key)
{
	while (env)
	{
		// strncmp だけでは "PATH" を検索したとき "PATH2" にも一致してしまう。
		// strlen を合わせて比較することで完全一致のみを通す。
		if (ft_strncmp(env->key, key, ft_strlen(key)) == 0
			&& ft_strlen(env->key) == ft_strlen(key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** t_env の1ノードから "KEY=VALUE" 形式の文字列を作って返す。
** value が NULL のノード（export NAME のみで登録された変数）は
** "KEY=" として扱う。
*/
static char	*make_kv_pair(t_env *node)
{
	char	*tmp_str;
	char	*kv;
	char	*val;

	tmp_str = ft_strjoin(node->key, "=");
	if (!tmp_str)
		return (NULL);
	val = node->value;
	if (!val)
		val ="";
	kv = ft_strjoin(tmp_str, val); // make_kv_pair が NULL を返す → fill_envp がそれを検知してエラー処理する、という設計になっているので、make_kv_pair の中で kv を個別にチェックする必要はない。
	free(tmp_str);
	return (kv);
}

/*
** t_env リストを走査して envp 配列を埋める。
** 失敗時は作成済みの要素を解放する（envp 配列自体は呼び出し元が解放する）。
*/
static int	fill_envp(t_env *env, char **envp)
{
	char	*kv;
	int		i;

	i = 0;
	while (env)
	{
		kv = make_kv_pair(env);
		if (!kv)
		{
			// while (i > 0) free(envp[--i]) : 前置デクリメントで i を1減らしてから
			// envp[i] を解放する。i == 0 になったら停止するので境界を越えない。
			while (i > 0)
				free(envp[--i]); // 作成済みの文字列を逆順に解放
			return (0);
		}
		envp[i] = kv;
		i++;
		env = env->next;
	}
	envp[i] = NULL; // 配列の終端マーク（execve が NULL で終わりを判断する）
	return (1);
}

/*
** t_env リストを char **envp 形式に変換して返す。
** execve に渡すために使う。
** 使い終わったら free_envp() で解放すること。
*/
char	**env_to_envp(t_env *env)
{
	char	**envp;
	t_env	*tmp;
	int		count;

	count = 0;
	tmp = env;
	while (tmp) // まずノード数をカウントして必要なサイズを確定させる
	{
		count++;
		tmp = tmp->next;
	}
	envp = malloc(sizeof(char *) * (count + 1)); // +1 は終端の NULL ポインタ用
	if (!envp)
		return (NULL);
	if (!fill_envp(env, envp))
	{
		free(envp); // 要素は fill_envp が解放済み、配列本体だけ解放
		return (NULL);
	}
	return (envp);
}

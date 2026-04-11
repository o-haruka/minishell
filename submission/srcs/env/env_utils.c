#include "minishell.h"
#include "libft.h"

/*
** t_env リスト全体を解放する。
** key・value・ノード本体の順で free する。
*/
void	free_env(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env->next;
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
		if (ft_strncmp(env->key, key, ft_strlen(key)) == 0
			&& ft_strlen(env->key) == ft_strlen(key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** t_env リストを char **envp 形式に変換して返す。
** execve に渡すために使う。
*/
char	**env_to_envp(t_env *env)
{
	t_env	*tmp;
	char	**envp;
	char	*kv;
	char	*tmp_str;
	int		count;
	int		i;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	envp = malloc(sizeof(char *) * (count + 1));
	if (!envp)
		return (NULL);
	i = 0;
	tmp = env;
	while (tmp)
	{
		tmp_str = ft_strjoin(tmp->key, "=");
		kv = ft_strjoin(tmp_str, tmp->value);
		free(tmp_str);
		envp[i] = kv;
		i++;
		tmp = tmp->next;
	}
	envp[i] = NULL;
	return (envp);
}

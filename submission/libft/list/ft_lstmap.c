/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:51:30 by homura            #+#    #+#             */
/*   Updated: 2025/11/08 18:45:50 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_list.h"

static int	add_new_node(t_list **new_list, t_list **new_tail, t_list *new_node)
{
	if (!*new_list)
	{
		*new_list = new_node;
		*new_tail = new_node;
	}
	else
	{
		(*new_tail)->next = new_node;
		*new_tail = new_node;
	}
	return (0);
}

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_node;
	t_list	*new_list;
	t_list	*new_tail;

	new_list = NULL;
	new_tail = NULL;
	if (!lst || !f || !del)
		return (NULL);
	while (lst)
	{
		new_node = ft_lstnew(f(lst->content));
		if (!new_node)
		{
			ft_lstclear(&new_list, del);
			return (NULL);
		}
		add_new_node(&new_list, &new_tail, new_node);
		lst = lst->next;
	}
	return (new_list);
}

// contentを大文字に変換する関数
// void *to_upper(void *content)
// {
//     char *str = (char *)content;
//     char *result = strdup(str);
//     for (int i = 0; result[i]; i++)
//         if (result[i] >= 'a' && result[i] <= 'z')
//             result[i] -= 32;
//     return (result);
// }

// // contentを解放する関数
// void del_content(void *content)
// {
//     free(content);
// }

// int main(void)
// {
//     // テストリストの作成
//     t_list *lst = ft_lstnew(strdup("hello"));
//     lst->next = ft_lstnew(strdup("world"));

//     // ft_lstmapを呼び出し
//     t_list *new_list = ft_lstmap(lst, to_upper, del_content);

//     // 結果を表示
//     t_list *current = new_list;
//     while (current)
//     {
//         printf("%s\n", (char *)current->content);
//         current = current->next;
//     }

//     // メモリ解放
//     ft_lstclear(&lst, del_content);
//     ft_lstclear(&new_list, del_content);
//     return (0);
// }
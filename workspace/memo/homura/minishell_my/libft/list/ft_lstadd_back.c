/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:50:59 by homura            #+#    #+#             */
/*   Updated: 2025/11/08 17:51:01 by homura           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_list.h"

// #include "libft.h"
// #include <stdlib.h>
// #include <stdio.h>

// typedef struct s_list
// {
//     void *content;
//     struct s_list *next;
// } t_list;

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next != NULL)
		last = last->next;
	last->next = new;
}

// リストを表示する補助関数
// void print_list(t_list *lst)
// {
//     while (lst)
//     {
//         printf("%s -> ", (char *)lst->content);
//         lst = lst->next;
//     }
//     printf("NULL\n");
// }

// int main(void)
// {
//     t_list *head = NULL;
//     t_list *node1 = malloc(sizeof(t_list));
//     t_list *node2 = malloc(sizeof(t_list));
//     t_list *node3 = malloc(sizeof(t_list));

//     // ノードの初期化
//     node1->content = "First";
//     node1->next = NULL;
//     node2->content = "Second";
//     node2->next = NULL;
//     node3->content = "Third";
//     node3->next = NULL;

//     // リストにノードを追加
//     ft_lstadd_back(&head, node1); // First
//     ft_lstadd_back(&head, node2); // First -> Second
//     ft_lstadd_back(&head, node3); // First -> Second -> Third

//     // リストを表示
//     print_list(head); // 出力: First -> Second -> Third -> NULL

//     // メモリ解放（簡略化のため省略可能）
//     free(node1);
//     free(node2);
//     free(node3);

//     return (0);
// }
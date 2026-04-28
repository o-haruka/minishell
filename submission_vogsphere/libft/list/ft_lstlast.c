/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:51:25 by homura            #+#    #+#             */
/*   Updated: 2025/11/08 17:51:27 by homura           ###   ########.fr       */
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

t_list	*ft_lstlast(t_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

// int main(void)
// {
//     // リスト作成: node1 -> node2 -> node3
//     t_list *node1 = malloc(sizeof(t_list));
//     t_list *node2 = malloc(sizeof(t_list));
//     t_list *node3 = malloc(sizeof(t_list));

//     node1->content = "A";
//     node1->next = node2;
//     node2->content = "B";
//     node2->next = node3;
//     node3->content = "C";
//     node3->next = NULL;

//     // テスト1: 通常のリスト
//     t_list *last = ft_lstlast(node1);
//     printf("Last node: %s\n", (char *)last->content); // 出力: C

//     // テスト2: 空リスト
//     last = ft_lstlast(NULL);
//     printf("Empty list: %p\n", last); // 出力: (nil)

//     // テスト3: 1ノード
//     node1->next = NULL;
//     last = ft_lstlast(node1);
//     printf("Single node: %s\n", (char *)last->content); // 出力: A

//     // メモリ解放
//     free(node1);
//     free(node2);
//     free(node3);
//     return (0);
// }
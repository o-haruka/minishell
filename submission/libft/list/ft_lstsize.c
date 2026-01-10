/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:51:41 by homura            #+#    #+#             */
/*   Updated: 2025/11/08 17:51:42 by homura           ###   ########.fr       */
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

int	ft_lstsize(t_list *list)
{
	int	count;

	count = 0;
	while (list)
	{
		count++;
		list = list->next;
	}
	return (count);
}

// ノードを作成するヘルパー関数
t_list	*create_node(void *content)
{
	t_list	*node;

	node = (t_list *)malloc(sizeof(t_list));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}

// リストを解放するヘルパー関数
void	free_list(t_list *lst)
{
	t_list	*temp;

	while (lst)
	{
		temp = lst;
		lst = lst->next;
		free(temp);
	}
}

// int main(void)
// {
//     // テストケース1: 3つのノードを持つリスト
//     t_list *list1 = create_node("data1");
//     list1->next = create_node("data2");
//     list1->next->next = create_node("data3");

//     printf("Test 1: List with 3 nodes\n");
//     printf("Node count: %d\n", ft_lstsize(list1)); // 期待値: 3

//     // テストケース2: 単一ノードのリスト
//     t_list *list2 = create_node("single");

//     printf("\nTest 2: List with 1 node\n");
//     printf("Node count: %d\n", ft_lstsize(list2)); // 期待値: 1

//     // テストケース3: 空リスト
//     t_list *list3 = NULL;

//     printf("\nTest 3: Empty list\n");
//     printf("Node count: %d\n", ft_lstsize(list3)); // 期待値: 0

//     // メモリ解放
//     free_list(list1);
//     free_list(list2);
//     // list3 は NULL なので解放不要

//     return (0);
// }
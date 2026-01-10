/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:51:10 by homura            #+#    #+#             */
/*   Updated: 2025/11/08 17:51:11 by homura           ###   ########.fr       */
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

void	del_content(void *content)
{
	free(content);
}

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*current;
	t_list	*next;

	if (!lst || !*lst || !del)
		return ;
	current = *lst;
	while (current)
	{
		next = current->next;
		del(current->content);
		free(current);
		current = next;
	}
	*lst = NULL;
}

// int main(void){
//     t_list *node1 = malloc(sizeof(t_list));
//     t_list *node2 = malloc(sizeof(t_list));
//     node1->content = malloc(sizeof(int));
//     node2->content = malloc(sizeof(int));
//     node1->next = node2;
//     node2->next = NULL;

//     ft_lstclear(&node1, del_content);
//     printf("List is %p\n", node1); // node1はNULLになる
//     return (0);
// }
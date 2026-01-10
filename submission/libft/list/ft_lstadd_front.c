/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homura <homura@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:51:05 by homura            #+#    #+#             */
/*   Updated: 2025/11/08 17:51:06 by homura           ###   ########.fr       */
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

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (lst == NULL || new == NULL)
		return ;
	new->next = *lst;
	*lst = new;
}

// int main(void) {
//     t_list *head = NULL;
//     t_list *node1 = malloc(sizeof(t_list));
//     t_list *node2 = malloc(sizeof(t_list));
//     node1->content = "A";
//     node1->next = node2;
//     node2->content = "B";
//     node2->next = NULL;
//     head = node1;

//     printf("before:  ");
//     t_list *temp = head;
//     while (temp)
//     {
//         printf("%s -> ", (char *)temp->content);
//         temp = temp->next;
//     }
//     printf("NULL\n");

//     t_list *new;
//     new = malloc(sizeof(t_list));
//     if(new == NULL)
//         return (1);
//     new->content = "C";
//     new->next = NULL;
//     ft_lstadd_front(&head, new);
//     printf("After:  ");
//     temp = head;
//     while (temp)
//     {
//         printf("%s -> ", (char *)temp->content);
//         temp = temp->next;
//     }

//     return (0);
// }
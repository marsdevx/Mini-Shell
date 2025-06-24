/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/06/24 20:24:55 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*node;

	node = malloc(sizeof(t_list));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (!lst || !new)
		return ;
	if (!*lst)
		*lst = new;
	else
	{
		last = *lst;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}

void	ft_free_tokens(t_list **tokens)
{
	t_list	*current;
	t_list	*next;
	t_token	*token;

	current = *tokens;
	while (current)
	{
		next = current->next;
		token = (t_token *)current->content;
		free(token->value);
		free(token);
		free(current);
		current = next;
	}
	*tokens = NULL;
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;

	dup = malloc(n + 1);
	if (!dup)
		return (NULL);
	strncpy(dup, s, n);
	dup[n] = '\0';
	return (dup);
}

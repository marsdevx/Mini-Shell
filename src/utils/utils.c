/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/06/26 18:13:38 by dkot             ###   ########.fr       */
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

	if (!tokens || !*tokens)
		return ;
	current = *tokens;
	while (current)
	{
		next = current->next;
		token = (t_token *)current->content;
		if (token)
		{
			if (token->value)
				free(token->value);
			free(token);
		}
		free(current);
		current = next;
	}
	*tokens = NULL;
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;
	size_t	i;

	if (!s)
		return (NULL);
	dup = malloc(n + 1);
	if (!dup)
		return (NULL);
	i = 0;
	while (i < n && s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

int ft_strcmp(const char *s1, const char *s2)
{
    // Memory safety checks
    if (s1 == NULL && s2 == NULL)
        return (0);     // Both NULL, considered equal
    if (s1 == NULL)
        return (INT_MIN); // s1 is NULL, s2 is not
    if (s2 == NULL)
        return (INT_MAX); // s2 is NULL, s1 is not
    
    // Compare characters until difference found or end reached
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    
    // Return difference of characters (unsigned char cast prevents sign issues)
    return ((unsigned char)*s1 - (unsigned char)*s2);
}

void	write_error(const char *msg)
{
	write(STDERR_FILENO, msg, ft_strlen(msg));
}

// Helper function to write error with variable part
void	write_error_with_arg(const char *prefix, const char *arg, const char *suffix)
{
	write(STDERR_FILENO, prefix, ft_strlen(prefix));
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, suffix, ft_strlen(suffix));
}

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}

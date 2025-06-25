/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 22:15:29 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

int ft_setenv(const char *name, const char *value, int overwrite)
{
    extern char **environ;
    size_t    len;
    char     *pair;
    int       i;
    char    **new_env;

    if (!name || !*name || ft_strchr(name, '='))
        return (-1);
    if (!value)
        value = "";

    len = ft_strlen(name);

    for (i = 0; environ[i]; i++)
    {
        if (ft_strncmp(environ[i], name, len) == 0
            && environ[i][len] == '=')
        {
            if (!overwrite)
                return (0);

            pair = ft_strjoin(name, "=");
            if (!pair)
                return (-1);
            {
                char *tmp = ft_strjoin(pair, value);
                free(pair);
                if (!tmp)
                    return (-1);
                free(environ[i]);
                environ[i] = tmp;
            }
            return (0);
        }
    }

    pair = ft_strjoin(name, "=");
    if (!pair)
        return (-1);
    {
        char *tmp = ft_strjoin(pair, value);
        free(pair);
        if (!tmp)
            return (-1);
        pair = tmp;
    }

    new_env = malloc(sizeof(char *) * (i + 2));
    if (!new_env)
    {
        free(pair);
        return (-1);
    }
		
    for (int j = 0; j < i; j++)
        new_env[j] = environ[j];
    new_env[i]     = pair;
    new_env[i + 1] = NULL;

    environ = new_env;
    return (0);
}

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

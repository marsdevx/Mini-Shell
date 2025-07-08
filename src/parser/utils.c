/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:31:52 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

t_command	*new_command(const char *arg)
{
	t_command	*c;

	c = malloc(sizeof(*c));
	if (!c)
		return (NULL);
	c->arg = ft_strdup(arg);
	if (!c->arg)
	{
		free(c);
		return (NULL);
	}
	return (c);
}

int	add_argument(t_group *grp, const char *arg)
{
	t_command	*cmd;
	t_list		*node;

	cmd = new_command(arg);
	if (!cmd)
		return (0);
	node = ft_lstnew(cmd);
	if (!node)
	{
		free(cmd->arg);
		free(cmd);
		return (0);
	}
	ft_lstadd_back(&grp->argv, node);
	return (1);
}

static void	copy_string(char *dest, char *src, int *index)
{
	int	i;

	i = 0;
	while (src && src[i])
	{
		dest[*index] = src[i];
		(*index)++;
		i++;
	}
}

char	*join_strings(char *s1, char *s2)
{
	char	*result;
	int		len1;
	int		len2;
	int		index;

	if (!s1 && !s2)
		return (NULL);
	len1 = 0;
	len2 = 0;
	if (s1)
		len1 = ft_strlen(s1);
	if (s2)
		len2 = ft_strlen(s2);
	result = malloc(len1 + len2 + 1);
	if (!result)
		return (NULL);
	index = 0;
	copy_string(result, s1, &index);
	copy_string(result, s2, &index);
	result[index] = '\0';
	return (result);
}

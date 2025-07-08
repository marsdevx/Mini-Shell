/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 18:06:33 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:12:00 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/27 18:21:07 by dkot             ###   ########.fr       */
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

t_group	*new_group(void)
{
	t_group	*g;

	g = malloc(sizeof(*g));
	if (!g)
		return (NULL);
	g->argv = NULL;
	return (g);
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

void	free_groups(t_list **groups)
{
	t_list	*nextg;
	t_group	*grp;
	t_list	*nexta;

	while (*groups)
	{
		nextg = (*groups)->next;
		grp = (*groups)->content;
		while (grp->argv)
		{
			nexta = grp->argv->next;
			free(((t_command *)grp->argv->content)->arg);
			free(grp->argv->content);
			free(grp->argv);
			grp->argv = nexta;
		}
		free(grp);
		free(*groups);
		*groups = nextg;
	}
}

char	*join_strings(char *s1, char *s2)
{
	char	*result;
	int		len1;
	int		len2;
	int		i;
	int		j;

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
	i = 0;
	while (s1 && i < len1)
	{
		result[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2 && j < len2)
	{
		result[i + j] = s2[j];
		j++;
	}
	result[i + j] = '\0';
	return (result);
}

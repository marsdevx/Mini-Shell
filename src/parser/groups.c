/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_groups.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 18:30:33 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:32:01 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

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

t_group	*new_group(void)
{
	t_group	*g;

	g = malloc(sizeof(*g));
	if (!g)
		return (NULL);
	g->argv = NULL;
	return (g);
}

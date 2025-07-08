/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_handlers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 18:07:42 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:21:51 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	handle_redirect_create_group(t_group **cur, t_list **groups)
{
	t_list	*group_node;

	*cur = new_group();
	if (!*cur)
		return (0);
	group_node = ft_lstnew(*cur);
	if (!group_node)
	{
		free(*cur);
		return (0);
	}
	ft_lstadd_back(groups, group_node);
	return (1);
}

int	handle_redirect(t_token *tk, t_list *tok_lst, t_group **cur,
		t_list **groups)
{
	t_list	*look;

	look = tok_lst->next;
	if (look && ((t_token *)look->content)->type == SEP)
		look = look->next;
	if (!look || !is_text(((t_token *)look->content)->type))
	{
		write(2, "syntax error: redirect \"", 24);
		write(2, tk->value, ft_strlen(tk->value));
		write(2, "\" needs a filename\n", 19);
		return (0);
	}
	if (!*cur)
	{
		if (!handle_redirect_create_group(cur, groups))
			return (0);
	}
	return (add_argument(*cur, tk->value));
}

int	handle_text_create_group(t_group **cur, t_list **groups)
{
	t_list	*group_node;

	*cur = new_group();
	if (!*cur)
		return (0);
	group_node = ft_lstnew(*cur);
	if (!group_node)
	{
		free(*cur);
		return (0);
	}
	ft_lstadd_back(groups, group_node);
	return (1);
}

int	handle_text(t_list **tok_lst, t_group **cur, t_list **groups,
		char **env)
{
	char	*arg;
	t_list	*scan;

	if (!*cur)
	{
		if (!handle_text_create_group(cur, groups))
			return (0);
	}
	scan = *tok_lst;
	arg = concatenate_text_tokens(&scan, env);
	if (!arg)
		return (0);
	if (ft_strlen(arg) > 0)
	{
		if (!add_argument(*cur, arg))
		{
			free(arg);
			return (0);
		}
	}
	free(arg);
	*tok_lst = scan;
	return (1);
}

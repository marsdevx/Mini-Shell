/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:24:12 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	handle_redirect_token(t_token *tk, t_list *tok_lst, t_group **cur,
	t_list **groups)
{
	if (!handle_redirect(tk, tok_lst, cur, groups))
	{
		free_groups(groups);
		return (0);
	}
	return (1);
}

static int	handle_text_token(t_list **tok_lst, t_group **cur,
	t_list **groups, char **env)
{
	if (!handle_text(tok_lst, cur, groups, env))
	{
		free_groups(groups);
		return (0);
	}
	return (1);
}

static int	handle_token_types(t_list **tok_lst, t_group **cur,
	t_list **groups, char **env)
{
	t_token	*tk;

	tk = (*tok_lst)->content;
	if (is_redirect(tk->type))
	{
		if (!handle_redirect_token(tk, *tok_lst, cur, groups))
			return (0);
		*tok_lst = (*tok_lst)->next;
	}
	else if (tk->type == PIPE)
	{
		*cur = NULL;
		*tok_lst = (*tok_lst)->next;
	}
	else if (tk->type == SEP)
		*tok_lst = (*tok_lst)->next;
	else if (is_text(tk->type))
	{
		if (!handle_text_token(tok_lst, cur, groups, env))
			return (0);
	}
	else
		*tok_lst = (*tok_lst)->next;
	return (1);
}

t_list	*tokens_to_groups(t_list *tok_lst, char **env)
{
	t_list	*groups;
	t_group	*cur;

	groups = NULL;
	cur = NULL;
	while (tok_lst)
	{
		if (!handle_token_types(&tok_lst, &cur, &groups, env))
			return (NULL);
	}
	return (groups);
}

t_list	*parser(t_list *tokens, char **env)
{
	t_list	*groups;

	if (!tokens)
		return (NULL);
	groups = tokens_to_groups(tokens, env);
	if (!groups)
		return (NULL);
	return (groups);
}

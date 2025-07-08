/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:06:17 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	is_redirect(t_e_type t)
{
	return (t == REDIRECT_IN || t == REDIRECT_OUT || t == REDIRECT_APPEND
		|| t == HEREDOC);
}

int	is_text(t_e_type t)
{
	return (t == WORD || t == FIELD || t == EXP_FIELD);
}

int	is_valid_var_char(char c, int first)
{
	if (first)
		return (ft_isalpha(c) || c == '_');
	return (ft_isalnum(c) || c == '_');
}

t_list	*tokens_to_groups(t_list *tok_lst, char **env)
{
	t_list	*groups;
	t_group	*cur;
	t_token	*tk;

	groups = NULL;
	cur = NULL;
	while (tok_lst)
	{
		tk = tok_lst->content;
		if (is_redirect(tk->type))
		{
			if (!handle_redirect(tk, tok_lst, &cur, &groups))
			{
				free_groups(&groups);
				return (NULL);
			}
			tok_lst = tok_lst->next;
		}
		else if (tk->type == PIPE)
		{
			cur = NULL;
			tok_lst = tok_lst->next;
		}
		else if (tk->type == SEP)
		{
			tok_lst = tok_lst->next;
		}
		else if (is_text(tk->type))
		{
			if (!handle_text(&tok_lst, &cur, &groups, env))
			{
				free_groups(&groups);
				return (NULL);
			}
		}
		else
			tok_lst = tok_lst->next;
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

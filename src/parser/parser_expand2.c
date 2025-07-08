/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expand2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:29:32 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static char	*expand_word_env_dollar(const char **p, char **result, char **env)
{
	if (is_valid_var_char((*p)[1], 1) || (*p)[1] == '?')
	{
		(*p)++;
		return (expand_word_env_process_var(p, result, env));
	}
	else if (!is_valid_var_char((*p)[1], 1) && (*p)[1] != '?')
	{
		return (expand_word_env_process_char(p, result, **p));
	}
	return (*result);
}

char	*expand_word_env(const char *src, char **env)
{
	const char	*p;
	char		*result;

	p = src;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (*p)
	{
		if (*p == '$')
		{
			result = expand_word_env_dollar(&p, &result, env);
			if (!result)
				return (NULL);
		}
		else
		{
			result = expand_word_env_process_char(&p, &result, *p);
			if (!result)
				return (NULL);
		}
	}
	return (result);
}

static char	*get_token_piece(t_token *tk, char **env)
{
	if (tk->type == EXP_FIELD || tk->type == WORD)
		return (expand_word_env(tk->value, env));
	else
		return (ft_strdup(tk->value));
}

static char	*append_piece(char *arg, char *piece)
{
	char	*new_arg;

	new_arg = join_strings(arg, piece);
	free(arg);
	free(piece);
	return (new_arg);
}

char	*concatenate_text_tokens(t_list **scan, char **env)
{
	char	*arg;
	char	*piece;
	t_token	*tk2;

	arg = ft_strdup("");
	if (!arg)
		return (NULL);
	while (*scan && is_text(((t_token *)(*scan)->content)->type))
	{
		tk2 = (*scan)->content;
		piece = get_token_piece(tk2, env);
		if (!piece)
		{
			free(arg);
			return (NULL);
		}
		arg = append_piece(arg, piece);
		if (!arg)
			return (NULL);
		*scan = (*scan)->next;
	}
	return (arg);
}

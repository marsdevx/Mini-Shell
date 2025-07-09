/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 21:57:54 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	handle_whitespace(char **ptr, t_token *token)
{
	char	*start;

	start = *ptr;
	while (**ptr == ' ' || **ptr == '\t')
		(*ptr)++;
	token->type = SEP;
	token->value = ft_strndup(start, 1);
	if (!token->value)
		return (0);
	token->value_len = 1;
	return (1);
}

int	handle_pipe(char **ptr, t_token *token)
{
	token->type = PIPE;
	token->value = ft_strndup(*ptr, 1);
	if (!token->value)
		return (0);
	token->value_len = 1;
	(*ptr)++;
	return (1);
}

int	handle_redirect_in(char **ptr, t_token *token)
{
	if (*(*ptr + 1) == '<')
	{
		token->type = HEREDOC;
		token->value = ft_strndup(*ptr, 2);
		if (!token->value)
			return (0);
		token->value_len = 2;
		*ptr += 2;
	}
	else
	{
		token->type = REDIRECT_IN;
		token->value = ft_strndup(*ptr, 1);
		if (!token->value)
			return (0);
		token->value_len = 1;
		(*ptr)++;
	}
	return (1);
}

int	handle_redirect_out(char **ptr, t_token *token)
{
	if (*(*ptr + 1) == '>')
	{
		token->type = REDIRECT_APPEND;
		token->value = ft_strndup(*ptr, 2);
		if (!token->value)
			return (0);
		token->value_len = 2;
		*ptr += 2;
	}
	else
	{
		token->type = REDIRECT_OUT;
		token->value = ft_strndup(*ptr, 1);
		if (!token->value)
			return (0);
		token->value_len = 1;
		(*ptr)++;
	}
	return (1);
}

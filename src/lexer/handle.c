/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 19:06:39 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	handle_whitespace(char **ptr, t_token *token)
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

static int	handle_pipe(char **ptr, t_token *token)
{
	token->type = PIPE;
	token->value = ft_strndup(*ptr, 1);
	if (!token->value)
		return (0);
	token->value_len = 1;
	(*ptr)++;
	return (1);
}

static int	handle_redirect_in(char **ptr, t_token *token)
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

static int	handle_redirect_out(char **ptr, t_token *token)
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

static int	handle_single_quote(char **ptr, t_token *token)
{
	char	*start;
	size_t	len;

	(*ptr)++;
	start = *ptr;
	while (**ptr && **ptr != '\'')
		(*ptr)++;
	if (**ptr != '\'')
		return (0);
	len = *ptr - start;
	token->type = FIELD;
	token->value = ft_strndup(start, len);
	if (!token->value)
		return (0);
	token->value_len = len;
	(*ptr)++;
	return (1);
}

static int	handle_double_quote(char **ptr, t_token *token)
{
	char	*start;
	size_t	len;

	(*ptr)++;
	start = *ptr;
	while (**ptr && **ptr != '"')
		(*ptr)++;
	if (**ptr != '"')
		return (0);
	len = *ptr - start;
	token->type = EXP_FIELD;
	token->value = ft_strndup(start, len);
	if (!token->value)
		return (0);
	token->value_len = len;
	(*ptr)++;
	return (1);
}

static int	handle_word(char **ptr, t_token *token)
{
	char	*start;
	size_t	len;

	start = *ptr;
	while (**ptr && **ptr != ' ' && **ptr != '\t' && **ptr != '|'
		&& **ptr != '<' && **ptr != '>' && **ptr != '\'' && **ptr != '"')
		(*ptr)++;
	len = *ptr - start;
	token->type = WORD;
	token->value = ft_strndup(start, len);
	if (!token->value)
		return (0);
	token->value_len = len;
	return (1);
}

int	process_token(char **ptr, t_token *token)
{
	if (**ptr == ' ' || **ptr == '\t')
		return (handle_whitespace(ptr, token));
	else if (**ptr == '|')
		return (handle_pipe(ptr, token));
	else if (**ptr == '<')
		return (handle_redirect_in(ptr, token));
	else if (**ptr == '>')
		return (handle_redirect_out(ptr, token));
	else if (**ptr == '\'')
		return (handle_single_quote(ptr, token));
	else if (**ptr == '"')
		return (handle_double_quote(ptr, token));
	else
		return (handle_word(ptr, token));
}

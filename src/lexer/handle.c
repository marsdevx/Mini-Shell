/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 21:53:12 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	handle_single_quote(char **ptr, t_token *token)
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

int	handle_double_quote(char **ptr, t_token *token)
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

int	handle_word(char **ptr, t_token *token)
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

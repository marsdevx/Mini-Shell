/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/07/10 17:44:43 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

t_token	*create_token(void)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = NULL;
	token->value_len = 0;
	return (token);
}

int	add_token_to_list(t_list **tokens, t_token *token)
{
	t_list	*new_node;

	new_node = ft_lstnew(token);
	if (!new_node)
	{
		free_token(token);
		return (0);
	}
	ft_lstadd_back(tokens, new_node);
	return (1);
}

int	process_single_token(char **ptr, t_list **tokens)
{
	t_token	*token;

	token = create_token();
	if (!token)
		return (0);
	if (!process_token(ptr, token))
	{
		free_token(token);
		return (0);
	}
	if (!add_token_to_list(tokens, token))
		return (0);
	return (1);
}

t_list	*tokenize_input(char *input)
{
	t_list	*tokens;
	char	*ptr;

	tokens = NULL;
	ptr = input;
	while (*ptr)
	{
		if (*ptr == ';' || *ptr == '\\')
		{
			++ptr;
			continue ;
		}
		if (!process_single_token(&ptr, &tokens))
		{
			cleanup_tokens(&tokens);
			return (NULL);
		}
		if (*ptr == '\0')
			break ;
	}
	return (tokens);
}

t_list	*lexer(char *input)
{
	t_list	*tokens;

	if (!quotes_check(input))
		return (NULL);
	tokens = tokenize_input(input);
	if (!tokens)
		return (NULL);
	if (!validate_pipe_syntax_enhanced(tokens))
	{
		cleanup_tokens(&tokens);
		return (NULL);
	}
	return (tokens);
}

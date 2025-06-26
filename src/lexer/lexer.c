/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/06/26 17:07:39 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	quotes_check(char *input)
{
	int	i;
	int	in_single_quote;
	int	in_double_quote;

	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (input[i])
	{
		if (input[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (input[i] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		i++;
	}
	if (in_single_quote || in_double_quote)
	{
		printf("Error: Unmatched quotes\n");
		return (0);
	}
	return (1);
}

void	free_token(t_token *token)
{
	if (token)
	{
		if (token->value)
			free(token->value);
		free(token);
	}
}

void	cleanup_tokens(t_list **tokens)
{
	if (tokens && *tokens)
	{
		ft_free_tokens(tokens);
	}
}

static t_token	*create_token(void)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = NULL;
	token->value_len = 0;
	return (token);
}

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

static int	process_token(char **ptr, t_token *token)
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

static int	add_token_to_list(t_list **tokens, t_token *token)
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

t_list	*lexer(char *input)
{
	t_list	*tokens;
	char	*ptr;
	t_token	*token;

	if (!quotes_check(input))
		return (NULL);
	tokens = NULL;
	ptr = input;
	while (*ptr)
	{
		token = create_token();
		if (!token)
		{
			cleanup_tokens(&tokens);
			return (NULL);
		}
		if (!process_token(&ptr, token))
		{
			free_token(token);
			cleanup_tokens(&tokens);
			return (NULL);
		}
		if (!add_token_to_list(&tokens, token))
		{
			cleanup_tokens(&tokens);
			return (NULL);
		}
		if (*ptr == '\0')
			break ;
	}
	return (tokens);
}
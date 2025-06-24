/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/06/24 19:52:14 by marksylaiev      ###   ########.fr       */
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

t_list *lexer(char *input)
{
    if (!quotes_check(input))
        return NULL;

    t_list *tokens = NULL;
    char *ptr = input;

    while (*ptr)
    {
        char *start = ptr;
        t_token *token = malloc(sizeof(t_token));
        if (!token)
        {
            cleanup_tokens(&tokens);
            return NULL;
        }

        token->value = NULL;
        token->value_len = 0;

        if (*ptr == ' ' || *ptr == '\t')
        {
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;
            token->type = SEP;
            token->value = ft_strndup(start, 1);
            if (!token->value)
            {
                free(token);
                cleanup_tokens(&tokens);
                return NULL;
            }
            token->value_len = 1;
        }
        else if (*ptr == '|')
        {
            token->type = PIPE;
            token->value = ft_strndup(ptr, 1);
            if (!token->value)
            {
                free(token);
                cleanup_tokens(&tokens);
                return NULL;
            }
            token->value_len = 1;
            ptr++;
        }
        else if (*ptr == '<')
        {
            if (*(ptr + 1) == '<')
            {
                token->type = HEREDOC;
                token->value = ft_strndup(ptr, 2);
                if (!token->value)
                {
                    free(token);
                    cleanup_tokens(&tokens);
                    return NULL;
                }
                token->value_len = 2;
                ptr += 2;
            }
            else
            {
                token->type = REDIRECT_IN;
                token->value = ft_strndup(ptr, 1);
                if (!token->value)
                {
                    free(token);
                    cleanup_tokens(&tokens);
                    return NULL;
                }
                token->value_len = 1;
                ptr++;
            }
        }
        else if (*ptr == '>')
        {
            if (*(ptr + 1) == '>')
            {
                token->type = REDIRECT_APPEND;
                token->value = ft_strndup(ptr, 2);
                if (!token->value)
                {
                    free(token);
                    cleanup_tokens(&tokens);
                    return NULL;
                }
                token->value_len = 2;
                ptr += 2;
            }
            else
            {
                token->type = REDIRECT_OUT;
                token->value = ft_strndup(ptr, 1);
                if (!token->value)
                {
                    free(token);
                    cleanup_tokens(&tokens);
                    return NULL;
                }
                token->value_len = 1;
                ptr++;
            }
        }
        else if (*ptr == '\'')
        {
            ptr++;
            start = ptr;
            while (*ptr && *ptr != '\'')
                ptr++;
            if (*ptr == '\'')
            {
                size_t len = ptr - start;
                token->type = FIELD;
                token->value = ft_strndup(start, len);
                if (!token->value)
                {
                    free(token);
                    cleanup_tokens(&tokens);
                    return NULL;
                }
                token->value_len = len;
                ptr++;
            }
            else
            {
                free(token);
                cleanup_tokens(&tokens);
                return NULL;
            }
        }
        else if (*ptr == '"')
        {
            ptr++;
            start = ptr;
            while (*ptr && *ptr != '"')
                ptr++;
            if (*ptr == '"')
            {
                size_t len = ptr - start;
                token->type = EXP_FIELD;
                token->value = ft_strndup(start, len);
                if (!token->value)
                {
                    free(token);
                    cleanup_tokens(&tokens);
                    return NULL;
                }
                token->value_len = len;
                ptr++;
            }
            else
            {
                free(token);
                cleanup_tokens(&tokens);
                return NULL;
            }
        }
        else
        {
            while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '|' && 
                   *ptr != '<' && *ptr != '>' && *ptr != '\'' && *ptr != '"')
                ptr++;
            size_t len = ptr - start;
            token->type = WORD;
            token->value = ft_strndup(start, len);
            if (!token->value)
            {
                free(token);
                cleanup_tokens(&tokens);
                return NULL;
            }
            token->value_len = len;
        }

        t_list *new_node = ft_lstnew(token);
        if (!new_node)
        {
            free_token(token);
            cleanup_tokens(&tokens);
            return NULL;
        }
        ft_lstadd_back(&tokens, new_node);
        
        if (*ptr == '\0')
            break;
    }
    return tokens;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_validator_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 16:30:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2025/07/10 18:01:22 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	validate_enhanced_pipe_token(int *has_command_before,
	int *expecting_command, int *consecutive_pipes)
{
	(*consecutive_pipes)++;
	if (*consecutive_pipes > 1)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (0);
	}
	if (!*has_command_before)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (0);
	}
	*expecting_command = 1;
	*has_command_before = 0;
	return (1);
}

static void	handle_command_token(int *has_command_before,
	int *expecting_command,	int *consecutive_pipes)
{
	*has_command_before = 1;
	*expecting_command = 0;
	*consecutive_pipes = 0;
}

int	process_token_for_enhanced_validation(t_token *token,
	int *has_command_before, int *expecting_command, int *consecutive_pipes)
{
	if (token->type == SEP)
		return (1);
	if (token->type == PIPE)
	{
		if (!validate_enhanced_pipe_token(has_command_before,
				expecting_command, consecutive_pipes))
			return (0);
	}
	else if (is_command_token(token->type))
		handle_command_token(has_command_before, expecting_command,
			consecutive_pipes);
	else if (token->type == REDIRECT_IN || token->type == REDIRECT_OUT
		|| token->type == REDIRECT_APPEND || token->type == HEREDOC)
		*consecutive_pipes = 0;
	return (1);
}

static int	validate_tokens_loop(t_list *tokens)
{
	t_list	*current;
	t_token	*token;
	int		has_command_before;
	int		expecting_command;
	int		consecutive_pipes;

	current = tokens;
	has_command_before = 0;
	expecting_command = 0;
	consecutive_pipes = 0;
	while (current)
	{
		token = (t_token *)current->content;
		if (!process_token_for_enhanced_validation(token, &has_command_before,
				&expecting_command, &consecutive_pipes))
			return (0);
		current = current->next;
	}
	if (expecting_command)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	return (1);
}

int	validate_pipe_syntax_enhanced(t_list *tokens)
{
	if (!tokens)
		return (1);
	return (validate_tokens_loop(tokens));
}

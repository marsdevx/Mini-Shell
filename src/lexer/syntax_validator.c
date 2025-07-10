/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_validator.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 16:30:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2025/07/10 16:41:47 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	is_command_token(t_e_type type)
{
	return (type == WORD || type == FIELD || type == EXP_FIELD);
}

int	validate_pipe_syntax(t_list *tokens)
{
	t_list	*current;
	t_token	*token;
	int		has_command_before;
	int		expecting_command;

	if (!tokens)
		return (1);
	current = tokens;
	has_command_before = 0;
	expecting_command = 0;
	while (current)
	{
		token = (t_token *)current->content;
		if (token->type == SEP)
		{
			current = current->next;
			continue ;
		}
		if (token->type == PIPE)
		{
			if (!has_command_before)
			{
				printf("minishell: syntax error near unexpected token `|'\n");
				return (0);
			}
			expecting_command = 1;
			has_command_before = 0;
		}
		else if (is_command_token(token->type))
		{
			has_command_before = 1;
			expecting_command = 0;
		}
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
	t_list	*current;
	t_token	*token;
	int		has_command_before;
	int		expecting_command;
	int		consecutive_pipes;

	if (!tokens)
		return (1);
	current = tokens;
	has_command_before = 0;
	expecting_command = 0;
	consecutive_pipes = 0;
	while (current)
	{
		token = (t_token *)current->content;
		if (token->type == SEP)
		{
			current = current->next;
			continue ;
		}
		if (token->type == PIPE)
		{
			consecutive_pipes++;
			if (consecutive_pipes > 1)
			{
				printf("minishell: syntax error near unexpected token `|'\n");
				return (0);
			}
			if (!has_command_before)
			{
				printf("minishell: syntax error near unexpected token `|'\n");
				return (0);
			}
			expecting_command = 1;
			has_command_before = 0;
		}
		else if (is_command_token(token->type))
		{
			has_command_before = 1;
			expecting_command = 0;
			consecutive_pipes = 0;
		}
		else if (token->type == REDIRECT_IN || token->type == REDIRECT_OUT
			|| token->type == REDIRECT_APPEND || token->type == HEREDOC)
		{
			consecutive_pipes = 0;
		}
		current = current->next;
	}
	if (expecting_command)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	return (1);
}

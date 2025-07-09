/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 16:59:40 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	handle_input_redirect_case(char **argv, int *i, int *last_error)
{
	if (!argv[*i + 1])
	{
		write_error("syntax error: expected filename after '<'\n");
		return (-1);
	}
	if (handle_input_redirect(argv[*i + 1]) < 0)
		*last_error = -1;
	remove_argv_element(argv, *i);
	remove_argv_element(argv, *i);
	return (1);
}

static int	handle_output_redirect_case(char **argv, int *i)
{
	if (!argv[*i + 1])
	{
		write_error("syntax error: expected filename after '>'\n");
		return (-1);
	}
	if (handle_output_redirect(argv[*i + 1]) < 0)
		return (-1);
	remove_argv_element(argv, *i);
	remove_argv_element(argv, *i);
	return (1);
}

static int	handle_append_redirect_case(char **argv, int *i, int *last_error)
{
	if (!argv[*i + 1])
	{
		write_error("syntax error: expected filename after '>>'\n");
		return (-1);
	}
	if (handle_append_redirect(argv[*i + 1]) < 0)
		*last_error = -1;
	remove_argv_element(argv, *i);
	remove_argv_element(argv, *i);
	return (1);
}

static int	handle_heredoc_case(char **argv, int *i, int *last_error)
{
	if (!argv[*i + 1])
	{
		write_error("syntax error: expected delimiter after '<<'\n");
		return (-1);
	}
	if (handle_heredoc(argv[*i + 1]) < 0)
		*last_error = -1;
	remove_argv_element(argv, *i);
	remove_argv_element(argv, *i);
	return (1);
}

int	setup_redirections(char ***argv_ptr)
{
	char	**argv;
	int		i;
	int		last_error;
	int		handled;

	argv = *argv_ptr;
	i = 0;
	last_error = 0;
	while (argv[i])
	{
		handled = 0;
		if (ft_strcmp(argv[i], "<") == 0)
			handled = handle_input_redirect_case(argv, &i, &last_error);
		else if (ft_strcmp(argv[i], ">") == 0)
			handled = handle_output_redirect_case(argv, &i);
		else if (ft_strcmp(argv[i], ">>") == 0)
			handled = handle_append_redirect_case(argv, &i, &last_error);
		else if (ft_strcmp(argv[i], "<<") == 0)
			handled = handle_heredoc_case(argv, &i, &last_error);
		if (handled < 0)
			return (-1);
		if (!handled)
			i++;
	}
	return (last_error);
}

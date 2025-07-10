/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/10 15:18:47 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	handle_error_status(t_info *info)
{
	info->env = set_env_var(info->env, "?", "2", 1);
	info->last_exit_status = 2;
}

static void	update_exit_status(t_info *info, int exit_status)
{
	char	*exit_str;

	exit_str = ft_itoa(exit_status);
	if (exit_str)
	{
		info->env = set_env_var(info->env, "?", exit_str, 1);
		free(exit_str);
	}
	info->last_exit_status = exit_status;
}

void	process_input(char *line, t_info *info)
{
	t_list	*tokens;
	t_list	*groups;
	int		exit_status;

	if (!line || *line == '\0')
		return ;
	tokens = lexer(line);
	if (!tokens)
		return (handle_error_status(info));
	groups = parser(tokens, info->env);
	ft_free_tokens(&tokens);
	if (groups)
	{
		exit_status = execute_commands(groups, info);
		free_groups(&groups);
		update_exit_status(info, exit_status);
	}
	else
		handle_error_status(info);
}

static void	main_loop(t_info *info)
{
	char	*line;

	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	while (info->exit_f)
	{
		line = ft_readline("minishell> ");
		if (line == NULL)
		{
			printf("exit\n");
			break ;
		}
		if (*line)
			process_input(line, info);
		free(line);
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	t_info	info;

	(void)argc;
	(void)argv;
	info.exit_f = 1;
	info.last_exit_status = 0;
	info.env = init_env_copy(envp);
	if (!info.env)
		return (EXIT_FAILURE);
	init_shell_env(&info);
	main_loop(&info);
	free_env_copy(info.env);
	return (info.last_exit_status);
}

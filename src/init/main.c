/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/27 16:08:43 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

void	process_input(char *line, t_info *info)
{
	t_list	*tokens;
	t_list	*groups;
	int		exit_status;
	char	*exit_str;

	if (!line || *line == '\0')
		return ;
	tokens = lexer(line);
	if (!tokens)
	{
		info->env = set_env_var(info->env, "?", "2", 1);
		info->last_exit_status = 2;
		return ;
	}
	groups = parser(tokens, info->env);
	ft_free_tokens(&tokens);
	if (groups)
	{
		exit_status = execute_commands(groups, info);
		free_groups(&groups);
		exit_str = ft_itoa(exit_status);
		if (exit_str)
		{
			info->env = set_env_var(info->env, "?", exit_str, 1);
			free(exit_str);
		}
		info->last_exit_status = exit_status;
	}
	else
	{
		info->env = set_env_var(info->env, "?", "2", 1);
		info->last_exit_status = 2;
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	t_info	info;
	char	*line;
	char	*user;

	(void)argc;
	(void)argv;
	
	info.exit_f = 1;
	info.last_exit_status = 0;
	
	// Initialize environment copy
	info.env = init_env_copy(envp);
	if (!info.env)
		return (EXIT_FAILURE);
	
	// Set initial environment variables
	info.env = set_env_var(info.env, "?", "0", 1);
	
	if (!get_env_value(info.env, "USER"))
	{
		user = get_env_value(info.env, "LOGNAME");
		if (user)
			info.env = set_env_var(info.env, "USER", user, 1);
		else
			info.env = set_env_var(info.env, "USER", "user", 1);
	}
	
	if (!get_env_value(info.env, "HOME"))
		info.env = set_env_var(info.env, "HOME", "/home/user", 1);
	
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	
	while (info.exit_f)
	{
		line = ft_readline("minishell> ");
		if (line == NULL)
		{
			printf("exit\n");
			break ;
		}
		if (*line)
		{
			process_input(line, &info);
		}
		free(line);
	}
	
	// Clean up environment
	free_env_copy(info.env);
	
	return (info.last_exit_status);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 17:34:12 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

t_list	*g_tokens = NULL;
t_list	*g_groups = NULL;

void	cleanup_global_memory(void)
{
	if (g_tokens)
	{
		ft_free_tokens(&g_tokens);
		g_tokens = NULL;
	}
	if (g_groups)
	{
		free_groups(&g_groups);
		g_groups = NULL;
	}
}

void	process_input(char *line, t_info *info, char **envp)
{
	t_list	*tokens;
	t_list	*groups;
	int		exit_status;
	char	exit_str[16];

	if (!line || *line == '\0')
		return ;
	tokens = lexer(line);
	if (!tokens)
	{
		setenv("?", "2", 1);
		info->last_exit_status = 2;
		return ;
	}
	g_tokens = tokens;
	groups = parser(tokens);
	if (groups)
	{
		g_groups = groups;
		exit_status = execute_commands(groups, envp, info);
		snprintf(exit_str, sizeof(exit_str), "%d", exit_status);
		setenv("?", exit_str, 1);
		info->last_exit_status = exit_status;
		free_groups(&groups);
		g_groups = NULL;
	}
	else
	{
		setenv("?", "2", 1);
		info->last_exit_status = 2;
	}
	ft_free_tokens(&tokens);
	g_tokens = NULL;
}

int	main(int argc, char *argv[], char *envp[])
{
	t_info	info;
	char	*line;
	char	*user;

	(void)argc;
	(void)argv;
	if (ft_init(&info) != 0)
	{
		return (EXIT_FAILURE);
	}
	setenv("?", "0", 1);
	if (!getenv("USER"))
	{
		user = getenv("LOGNAME");
		if (user)
			setenv("USER", user, 1);
		else
			setenv("USER", "user", 1);
	}
	if (!getenv("HOME"))
		setenv("HOME", "/home/user", 1);
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
			process_input(line, &info, envp);
		}
		free(line);
	}
	cleanup_global_memory();
	return (info.last_exit_status);
}
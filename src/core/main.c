/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 22:15:25 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

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
		ft_setenv("?", "2", 1);
		info->last_exit_status = 2;
		return ;
	}
	groups = parser(tokens);
	if (groups)
	{
		exit_status = execute_commands(groups, envp, info);
		snprintf(exit_str, sizeof(exit_str), "%d", exit_status);
		ft_setenv("?", exit_str, 1);
		info->last_exit_status = exit_status;
		free_groups(&groups);
	}
	else
	{
		ft_setenv("?", "2", 1);
		info->last_exit_status = 2;
	}
	ft_free_tokens(&tokens);
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
	ft_setenv("?", "0", 1);
	if (!getenv("USER"))
	{
		user = getenv("LOGNAME");
		if (user)
			ft_setenv("USER", user, 1);
		else
			ft_setenv("USER", "user", 1);
	}
	if (!getenv("HOME"))
		ft_setenv("HOME", "/home/user", 1);
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
	return (info.last_exit_status);
}

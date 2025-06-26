/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/26 20:47:05 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	execute_external(char **args, t_exec_ctx *ctx)
{
	char		*cmd_path;
	struct stat	st;
	pid_t		pid;
	int			status;

	cmd_path = resolve_command_path(args[0], ctx->info->env);
	if (!cmd_path)
	{
		write_error_with_arg("bash: ", args[0], ": command not found\n");
		return (127);
	}
	if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		write_error_with_arg("bash: ", args[0], ": Is a directory\n");
		free(cmd_path);
		return (126);
	}
	if (access(cmd_path, X_OK) != 0 && access(cmd_path, F_OK) == 0)
	{
		write_error_with_arg("bash: ", args[0], ": Permission denied\n");
		free(cmd_path);
		return (126);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		free(cmd_path);
		return (1);
	}
	else if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execve(cmd_path, args, ctx->info->env);
		perror(args[0]);
		exit(126);
	}
	else
	{
		if (waitpid(pid, &status, 0) < 0)
		{
			perror("waitpid");
			free(cmd_path);
			return (1);
		}
		free(cmd_path);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			return (128 + WTERMSIG(status));
		else
			return (1);
	}
}
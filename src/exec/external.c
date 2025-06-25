/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 22:15:22 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

int	execute_external(char **args, t_exec_ctx *ctx)
{
	char		*cmd_path;
	struct stat	st;
	pid_t		pid;
	int			status;

	cmd_path = resolve_command_path(args[0]);
	if (!cmd_path)
	{
		fprintf(stderr, "bash: %s: command not found\n", args[0]);
		return (127);
	}
	if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		fprintf(stderr, "bash: %s: Is a directory\n", args[0]);
		free(cmd_path);
		return (126);
	}
	if (access(cmd_path, X_OK) != 0 && access(cmd_path, F_OK) == 0)
	{
		fprintf(stderr, "bash: %s: Permission denied\n", args[0]);
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
		execve(cmd_path, args, ctx->envp);
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 19:43:25 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	validate_command(char *cmd_path, char *cmd_name)
{
	struct stat	st;

	if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		write_error_with_arg("bash: ", cmd_name, ": Is a directory\n");
		return (126);
	}
	if (access(cmd_path, X_OK) != 0 && access(cmd_path, F_OK) == 0)
	{
		write_error_with_arg("bash: ", cmd_name, ": Permission denied\n");
		return (126);
	}
	return (0);
}

static void	child_process(char *cmd_path, char **args, char **env)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	execve(cmd_path, args, env);
	perror(args[0]);
	exit(127);
}

static int	get_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	else
		return (1);
}

static int	fork_and_execute(char *cmd_path, char **args, char **env)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	else if (pid == 0)
		child_process(cmd_path, args, env);
	else
	{
		if (waitpid(pid, &status, 0) < 0)
		{
			perror("waitpid");
			return (1);
		}
		return (get_exit_status(status));
	}
	return (1);
}

int	execute_external(char **args, t_exec_ctx *ctx)
{
	char	*cmd_path;
	int		validation_result;
	int		exit_status;

	cmd_path = resolve_command_path(args[0], ctx->info->env);
	if (!cmd_path)
	{
		write_error_with_arg("bash: ", args[0], ": command not found\n");
		return (127);
	}
	validation_result = validate_command(cmd_path, args[0]);
	if (validation_result != 0)
	{
		free(cmd_path);
		return (validation_result);
	}
	exit_status = fork_and_execute(cmd_path, args, ctx->info->env);
	free(cmd_path);
	return (exit_status);
}

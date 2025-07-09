/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:00:00 by assistant        #+#    #+#             */
/*   Updated: 2025/07/09 12:00:00 by assistant        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	exit_with_cleanup(char **argv, t_exec_ctx *ctx, t_pipe_data *data,
		int status)
{
	free_env_copy(ctx->info->env);
	free_groups(&data->tmp);
	free_argv(argv);
	free(data->pipes);
	free(data->pids);
	exit(status);
}

static void	handle_command_not_found(char **argv, t_exec_ctx *ctx,
		t_pipe_data *data)
{
	write_error_with_arg("bash: ", argv[0], ": command not found\n");
	exit_with_cleanup(argv, ctx, data, 127);
}

static void	handle_directory_error(char **argv, t_exec_ctx *ctx,
		t_pipe_data *data, char *cmd_path)
{
	write_error_with_arg("bash: ", argv[0], ": Is a directory\n");
	free(cmd_path);
	exit_with_cleanup(argv, ctx, data, 126);
}

static void	handle_permission_error(char **argv, t_exec_ctx *ctx,
		t_pipe_data *data, char *cmd_path)
{
	write_error_with_arg("bash: ", argv[0], ": Permission denied\n");
	free(cmd_path);
	exit_with_cleanup(argv, ctx, data, 126);
}

void	execute_external_child(char **argv, t_exec_ctx *ctx, t_pipe_data *data)
{
	char		*cmd_path;
	struct stat	st;

	cmd_path = resolve_command_path(argv[0], ctx->info->env);
	if (!cmd_path)
		handle_command_not_found(argv, ctx, data);
	if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
		handle_directory_error(argv, ctx, data, cmd_path);
	if (access(cmd_path, X_OK) != 0 && access(cmd_path, F_OK) == 0)
		handle_permission_error(argv, ctx, data, cmd_path);
	execve(cmd_path, argv, ctx->info->env);
	perror(argv[0]);
	exit_with_cleanup(argv, ctx, data, 126);
}

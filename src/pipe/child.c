/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:00:00 by assistant        #+#    #+#             */
/*   Updated: 2025/07/09 12:00:00 by assistant        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	exit_child(char **argv, t_exec_ctx *ctx, t_pipe_data *data,
		int status)
{
	free_env_copy(ctx->info->env);
	free_groups(&data->tmp);
	free_argv(argv);
	free(data->pipes);
	free(data->pids);
	exit(status);
}

static void	handle_empty_command(char **argv, t_exec_ctx *ctx,
		t_pipe_data *data)
{
	if (!argv[0] || ft_strlen(argv[0]) == 0)
		exit_child(argv, ctx, data, 0);
}

static void	handle_redirection_error(char **argv, t_exec_ctx *ctx,
		t_pipe_data *data, int redir_status)
{
	if (redir_status < 0)
		exit_child(argv, ctx, data, 1);
}

static void	execute_builtin_child(char **argv, t_exec_ctx *ctx,
		t_pipe_data *data)
{
	t_exec_ctx	child_ctx;
	t_info		child_info;
	int			status;

	child_ctx = *ctx;
	child_info = *ctx->info;
	child_ctx.info = &child_info;
	status = execute_builtin(argv, &child_ctx);
	exit_child(argv, ctx, data, status);
}

void	execute_child_process(char **argv, t_exec_ctx *ctx, t_pipe_data *data)
{
	int	redir_status;

	redir_status = setup_redirections(&argv);
	handle_empty_command(argv, ctx, data);
	handle_redirection_error(argv, ctx, data, redir_status);
	if (is_builtin(argv[0]))
		execute_builtin_child(argv, ctx, data);
	else
		execute_external_child(argv, ctx, data);
}

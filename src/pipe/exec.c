/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:00:00 by assistant        #+#    #+#             */
/*   Updated: 2025/07/09 12:00:00 by assistant        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	handle_fork_error(char **argv, t_pipe_data *data, int cmd_idx)
{
	int	i;

	perror("fork");
	free_argv(argv);
	i = 0;
	while (i < cmd_idx)
	{
		kill(data->pids[i], SIGTERM);
		waitpid(data->pids[i], NULL, 0);
		i++;
	}
	close_all_pipes(data->pipes, data->pipe_count - 1);
	free(data->pids);
	free(data->pipes);
}

static int	prepare_command(t_list *groups, t_pipe_data *data, char ***argv)
{
	t_group	*grp;

	grp = (t_group *)groups->content;
	*argv = group_to_argv(grp);
	if (!*argv)
	{
		handle_fork_error(NULL, data, data->cmd_index);
		return (1);
	}
	return (0);
}

static int	fork_process(t_pipe_data *data, char **argv, t_exec_ctx *ctx)
{
	data->pids[data->cmd_index] = fork();
	if (data->pids[data->cmd_index] < 0)
	{
		handle_fork_error(argv, data, data->cmd_index);
		return (1);
	}
	else if (data->pids[data->cmd_index] == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		setup_child_pipes(data);
		execute_child_process(argv, ctx, data);
	}
	return (0);
}

int	execute_pipe_commands(t_list *groups, t_exec_ctx *ctx, t_pipe_data *data)
{
	char	**argv;

	while (groups)
	{
		if (prepare_command(groups, data, &argv) != 0)
			return (1);
		if (fork_process(data, argv, ctx) != 0)
			return (1);
		free_argv(argv);
		groups = groups->next;
		data->cmd_index++;
	}
	return (0);
}

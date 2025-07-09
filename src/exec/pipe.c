/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 17:45:02 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	count_pipes(t_list *groups)
{
	int		count;
	t_list	*tmp;

	count = 0;
	tmp = groups;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	return (count);
}

static int	handle_single_command(t_list *groups, t_exec_ctx *ctx)
{
	t_group	*grp;

	grp = (t_group *)groups->content;
	return (execute_single_command(grp, ctx));
}

int	execute_pipeline(t_list *groups, t_exec_ctx *ctx)
{
	t_pipe_data	data;
	int			status;

	data.pipe_count = count_pipes(groups);
	if (data.pipe_count == 1)
		return (handle_single_command(groups, ctx));
	data.pipes = malloc(sizeof(int[2]) * (data.pipe_count - 1));
	if (!data.pipes)
		return (1);
	if (create_pipes(&data) != 0)
		return (1);
	data.pids = malloc(sizeof(pid_t) * data.pipe_count);
	if (!data.pids)
	{
		close_all_pipes(data.pipes, data.pipe_count - 1);
		free(data.pipes);
		return (1);
	}
	data.cmd_index = 0;
	data.tmp = groups;
	status = execute_pipe_commands(groups, ctx, &data);
	close_all_pipes(data.pipes, data.pipe_count - 1);
	status = wait_for_children(&data);
	free(data.pids);
	free(data.pipes);
	return (status);
}

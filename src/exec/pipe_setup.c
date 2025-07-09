/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_setup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:00:00 by assistant        #+#    #+#             */
/*   Updated: 2025/07/09 12:00:00 by assistant        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	create_pipes(t_pipe_data *data)
{
	int	i;
	int	j;

	i = 0;
	while (i < data->pipe_count - 1)
	{
		if (pipe(data->pipes[i]) < 0)
		{
			perror("pipe");
			j = 0;
			while (j < i)
			{
				close(data->pipes[j][0]);
				close(data->pipes[j][1]);
				j++;
			}
			free(data->pipes);
			return (1);
		}
		i++;
	}
	return (0);
}

void	close_all_pipes(int (*pipes)[2], int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

void	setup_child_pipes(t_pipe_data *data)
{
	if (data->cmd_index > 0)
		dup2(data->pipes[data->cmd_index - 1][0], STDIN_FILENO);
	if (data->cmd_index < data->pipe_count - 1)
		dup2(data->pipes[data->cmd_index][1], STDOUT_FILENO);
	close_all_pipes(data->pipes, data->pipe_count - 1);
}

int	wait_for_children(t_pipe_data *data)
{
	int	i;
	int	child_status;
	int	status;

	status = 0;
	i = 0;
	while (i < data->pipe_count)
	{
		if (waitpid(data->pids[i], &child_status, 0) < 0)
			perror("waitpid");
		else if (i == data->pipe_count - 1)
		{
			if (WIFEXITED(child_status))
				status = WEXITSTATUS(child_status);
			else if (WIFSIGNALED(child_status))
				status = 128 + WTERMSIG(child_status);
		}
		i++;
	}
	return (status);
}

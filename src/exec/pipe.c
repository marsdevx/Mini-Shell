/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/27 18:09:48 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	execute_pipeline(t_list *groups, t_exec_ctx *ctx)
{
	int pipe_count = 0;
	t_list *tmp = groups;

	while (tmp)
	{
		pipe_count++;
		tmp = tmp->next;
	}

	if (pipe_count == 1)
	{
		t_group *grp = (t_group *)groups->content;
		return (execute_single_command(grp, ctx));
	}

	int(*pipes)[2] = malloc(sizeof(int[2]) * (pipe_count - 1));
	if (!pipes)
		return (1);

	for (int i = 0; i < pipe_count - 1; i++)
	{
		if (pipe(pipes[i]) < 0)
		{
			perror("pipe");

			for (int j = 0; j < i; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			free(pipes);
			return (1);
		}
	}

	int cmd_index = 0;
	pid_t *pids = malloc(sizeof(pid_t) * pipe_count);
	if (!pids)
	{
		for (int i = 0; i < pipe_count - 1; i++)
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		free(pipes);
		return (1);
	}
	tmp = groups;
	while (groups)
	{
		t_group *grp = (t_group *)groups->content;
		char **argv = group_to_argv(grp);
		if (!argv)
		{
			for (int i = 0; i < cmd_index; i++)
			{
				kill(pids[i], SIGTERM);
				waitpid(pids[i], NULL, 0);
			}
			for (int i = 0; i < pipe_count - 1; i++)
			{
				close(pipes[i][0]);
				close(pipes[i][1]);
			}
			free(pids);
			free(pipes);
			return (1);
		}

		pids[cmd_index] = fork();
		if (pids[cmd_index] < 0)
		{
			perror("fork");
			free_argv(argv);

			for (int i = 0; i < cmd_index; i++)
			{
				kill(pids[i], SIGTERM);
				waitpid(pids[i], NULL, 0);
			}
			for (int i = 0; i < pipe_count - 1; i++)
			{
				close(pipes[i][0]);
				close(pipes[i][1]);
			}
			free(pids);
			free(pipes);
			return (1);
		}
		else if (pids[cmd_index] == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);

			if (cmd_index > 0)
			{
				dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
			}

			if (cmd_index < pipe_count - 1)
			{
				dup2(pipes[cmd_index][1], STDOUT_FILENO);
			}

			for (int i = 0; i < pipe_count - 1; i++)
			{
				close(pipes[i][0]);
				close(pipes[i][1]);
			}

			int redir_status = setup_redirections(&argv);

			if (!argv[0] || ft_strlen(argv[0]) == 0)
			{
				free_env_copy(ctx->info->env);
				free_groups(&tmp);
				free_argv(argv);
				free(pipes);
				free(pids);
				exit(0);
			}

			if (redir_status < 0)
			{
				free_env_copy(ctx->info->env);
				free_groups(&tmp);
				free_argv(argv);
				free(pipes);
				free(pids);
				exit(1);
			}

			if (is_builtin(argv[0]))
			{
				t_exec_ctx child_ctx = *ctx;

				t_info child_info = *ctx->info;
				child_ctx.info = &child_info;

				int status = execute_builtin(argv, &child_ctx);
				free_env_copy(ctx->info->env);
				free_groups(&tmp);
				free_argv(argv);
				free(pipes);
				free(pids);
				exit(status);
			}
			else
			{
				char *cmd_path = resolve_command_path(argv[0], ctx->info->env);
				if (!cmd_path)
				{
					write_error_with_arg("bash: ", argv[0], ": command not found\n");
					free_env_copy(ctx->info->env);
					free_groups(&tmp);
					free_argv(argv);
					free(pipes);
					free(pids);
					exit(127);
				}

				struct stat st;
				if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
				{
					write_error_with_arg("bash: ", argv[0], ": Is a directory\n");
					free(cmd_path);
					free_env_copy(ctx->info->env);
					free_groups(&tmp);
					free_argv(argv);
					free(pipes);
					free(pids);
					exit(126);
				}

				if (access(cmd_path, X_OK) != 0 && access(cmd_path, F_OK) == 0)
				{
					write_error_with_arg("bash: ", argv[0], ": Permission denied\n");
					free(cmd_path);
					free_env_copy(ctx->info->env);
					free_groups(&tmp);
					free_argv(argv);
					free(pipes);
					free(pids);
					exit(126);
				}

				execve(cmd_path, argv, ctx->info->env);
				perror(argv[0]);
				free_env_copy(ctx->info->env);
				free_groups(&tmp);
				free_argv(argv);
				free(pipes);
				free(pids);
				exit(126);
			}
		}

		free_argv(argv);
		groups = groups->next;
		cmd_index++;
	}

	for (int i = 0; i < pipe_count - 1; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	int status = 0;
	for (int i = 0; i < pipe_count; i++)
	{
		int child_status;
		if (waitpid(pids[i], &child_status, 0) < 0)
		{
			perror("waitpid");
		}
		else
		{
			if (i == pipe_count - 1)
			{
				if (WIFEXITED(child_status))
					status = WEXITSTATUS(child_status);
				else if (WIFSIGNALED(child_status))
					status = 128 + WTERMSIG(child_status);
			}
		}
	}

	free(pids);
	free(pipes);
	return (status);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/26 18:21:57 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

char	**group_to_argv(t_group *grp)
{
	int			count;
	char		**argv;
	int			i;
	t_list		*current;
	t_command	*cmd;

	count = count_args(grp->argv);
	argv = malloc(sizeof(char *) * (count + 1));
	if (!argv)
		return (NULL);
	i = 0;
	current = grp->argv;
	while (current)
	{
		cmd = (t_command *)current->content;
		argv[i] = ft_strdup(cmd->arg);
		if (!argv[i])
		{
			while (--i >= 0)
				free(argv[i]);
			free(argv);
			return (NULL);
		}
		i++;
		current = current->next;
	}
	argv[i] = NULL;
	return (argv);
}

int	count_args(t_list *args)
{
	int	count;

	count = 0;
	while (args)
	{
		count++;
		args = args->next;
	}
	return (count);
}

void	free_argv(char **argv)
{
	if (!argv)
		return ;
	for (int i = 0; argv[i]; i++)
		free(argv[i]);
	free(argv);
}

int	execute_commands(t_list *groups, char **envp, t_info *info)
{
	t_exec_ctx	ctx;
	int			group_count;
	t_list		*tmp;
	t_group		*grp;
	char		exit_str[16];

	ctx.stdin_backup = dup(STDIN_FILENO);
	ctx.stdout_backup = dup(STDOUT_FILENO);
	ctx.last_exit_status = info->last_exit_status;
	ctx.envp = envp;
	ctx.info = info;
	if (!groups)
	{
		close(ctx.stdin_backup);
		close(ctx.stdout_backup);
		return (info->last_exit_status);
	}
	group_count = 0;
	tmp = groups;
	while (tmp)
	{
		group_count++;
		tmp = tmp->next;
	}
	if (group_count > 1)
	{
		ctx.last_exit_status = execute_pipeline(groups, &ctx);
	}
	else if (groups)
	{
		grp = (t_group *)groups->content;
		ctx.last_exit_status = execute_single_command(grp, &ctx);
	}
	close(ctx.stdin_backup);
	close(ctx.stdout_backup);
	info->last_exit_status = ctx.last_exit_status;
	snprintf(exit_str, sizeof(exit_str), "%d", ctx.last_exit_status);
	setenv("?", exit_str, 1);
	return (ctx.last_exit_status);
}

int	execute_single_command(t_group *grp, t_exec_ctx *ctx)
{
	char	**argv;
	int		status;
	int		stdin_temp;
	int		stdout_temp;

	if (!grp || !grp->argv)
		return (0);
	argv = group_to_argv(grp);
	if (!argv)
		return (1);
	if (!argv[0])
	{
		free_argv(argv);
		return (0);
	}
	status = 0;
	stdin_temp = dup(STDIN_FILENO);
	stdout_temp = dup(STDOUT_FILENO);
	if (setup_redirections(&argv) < 0)
	{
		status = 1;
		goto cleanup;
	}
	if (!argv[0])
	{
		status = 0;
		goto cleanup;
	}
	if (strlen(argv[0]) == 0)
	{
		status = 0;
		goto cleanup;
	}
	if (is_builtin(argv[0]))
		status = execute_builtin(argv, ctx);
	else
		status = execute_external(argv, ctx);
cleanup:
	dup2(stdin_temp, STDIN_FILENO);
	dup2(stdout_temp, STDOUT_FILENO);
	close(stdin_temp);
	close(stdout_temp);
	free_argv(argv);
	return (status);
}

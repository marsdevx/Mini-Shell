/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 20:21:26 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	free_argv_on_error(char **argv, int i)
{
	while (--i >= 0)
		free(argv[i]);
	free(argv);
}

char	**group_to_argv(t_group *grp)
{
	char		**argv;
	int			i;
	t_list		*current;
	t_command	*cmd;

	argv = malloc(sizeof(char *) * (count_args(grp->argv) + 1));
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
			free_argv_on_error(argv, i);
			return (NULL);
		}
		i++;
		current = current->next;
	}
	argv[i] = NULL;
	return (argv);
}

static int	count_groups(t_list *groups)
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

int	execute_commands(t_list *groups, t_info *info)
{
	t_exec_ctx	ctx;
	t_group		*grp;
	char		*exit_str;

	ctx.stdin_backup = dup(STDIN_FILENO);
	ctx.stdout_backup = dup(STDOUT_FILENO);
	ctx.last_exit_status = info->last_exit_status;
	ctx.info = info;
	if (!groups)
	{
		close(ctx.stdin_backup);
		close(ctx.stdout_backup);
		return (info->last_exit_status);
	}
	if (count_groups(groups) > 1)
		ctx.last_exit_status = execute_pipeline(groups, &ctx);
	else
	{
		grp = (t_group *)groups->content;
		ctx.last_exit_status = execute_single_command(grp, &ctx);
	}
	close(ctx.stdin_backup);
	close(ctx.stdout_backup);
	info->last_exit_status = ctx.last_exit_status;
	exit_str = ft_itoa(ctx.last_exit_status);
	if (exit_str)
	{
		info->env = set_env_var(info->env, "?", exit_str, 1);
		free(exit_str);
	}
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
	stdin_temp = dup(STDIN_FILENO);
	stdout_temp = dup(STDOUT_FILENO);
	if (setup_redirections(&argv) < 0)
	{
		cleanup(stdin_temp, stdout_temp, argv);
		return (1);
	}
	if (!argv[0] || ft_strlen(argv[0]) == 0)
		status = 0;
	else if (is_builtin(argv[0]))
		status = execute_builtin(argv, ctx);
	else
		status = execute_external(argv, ctx);
	cleanup(stdin_temp, stdout_temp, argv);
	return (status);
}

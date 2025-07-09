/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 21:03:47 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

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

static void	init_exec_ctx(t_exec_ctx *ctx, t_info *info)
{
	ctx->stdin_backup = dup(STDIN_FILENO);
	ctx->stdout_backup = dup(STDOUT_FILENO);
	ctx->last_exit_status = info->last_exit_status;
	ctx->info = info;
}

static void	update_exit_status(t_exec_ctx *ctx, t_info *info)
{
	char	*exit_str;

	info->last_exit_status = ctx->last_exit_status;
	exit_str = ft_itoa(ctx->last_exit_status);
	if (exit_str)
	{
		info->env = set_env_var(info->env, "?", exit_str, 1);
		free(exit_str);
	}
}

int	execute_commands(t_list *groups, t_info *info)
{
	t_exec_ctx	ctx;
	t_group		*grp;

	init_exec_ctx(&ctx, info);
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
	update_exit_status(&ctx, info);
	return (ctx.last_exit_status);
}

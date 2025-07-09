/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 21:11:37 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static char	*get_cd_path(char **args, t_exec_ctx *ctx)
{
	char	*path;

	if (!args[1])
	{
		path = get_env_value(ctx->info->env, "HOME");
		if (!path)
			write_error("bash: cd: HOME not set\n");
		return (path);
	}
	if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value(ctx->info->env, "OLDPWD");
		if (!path)
			write_error("bash: cd: OLDPWD not set\n");
		else
			printf("%s\n", path);
		return (path);
	}
	return (args[1]);
}

static int	update_pwd_vars(t_exec_ctx *ctx, char *old_pwd)
{
	char	cwd[1024];

	if (old_pwd)
		ctx->info->env = set_env_var(ctx->info->env, "OLDPWD", old_pwd, 1);
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		ctx->info->env = set_env_var(ctx->info->env, "PWD", cwd, 1);
	return (0);
}

int	builtin_cd(char **args, t_exec_ctx *ctx)
{
	char	*path;
	char	cwd[1024];

	if (args[1] && args[2])
	{
		write_error("bash: cd: too many arguments\n");
		return (1);
	}
	path = get_cd_path(args, ctx);
	if (!path)
		return (1);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		cwd[0] = '\0';
	if (chdir(path) != 0)
	{
		write_error_with_arg("bash: cd: ", path,
			": No such file or directory\n");
		return (1);
	}
	if (cwd[0])
		return (update_pwd_vars(ctx, cwd));
	return (update_pwd_vars(ctx, NULL));
}

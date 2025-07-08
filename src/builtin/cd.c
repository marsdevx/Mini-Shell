/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:56:22 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	builtin_cd(char **args, t_exec_ctx *ctx)
{
	char	*path;
	char	cwd[1024];

	if (args[1] && args[2])
	{
		write_error("bash: cd: too many arguments\n");
		return (1);
	}
	if (!args[1])
	{
		path = get_env_value(ctx->info->env, "HOME");
		if (!path)
		{
			write_error("bash: cd: HOME not set\n");
			return (1);
		}
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value(ctx->info->env, "OLDPWD");
		if (!path)
		{
			write_error("bash: cd: OLDPWD not set\n");
			return (1);
		}
		printf("%s\n", path);
	}
	else
	{
		path = args[1];
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		ctx->info->env = set_env_var(ctx->info->env, "OLDPWD", cwd, 1);
	if (chdir(path) != 0)
	{
		write_error_with_arg("bash: cd: ", path, ": No such file or directory\n");
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		ctx->info->env = set_env_var(ctx->info->env, "PWD", cwd, 1);
	return (0);
}

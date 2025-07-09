/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 21:13:42 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	print_env_vars(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		printf("declare -x %s\n", env[i]);
		i++;
	}
}

static int	handle_var_with_value(char *arg, char *equals, t_exec_ctx *ctx)
{
	char	*var_name;
	int		len;

	*equals = '\0';
	if (!is_valid_identifier(arg))
	{
		write_error_with_arg("bash: export: `", arg,
			"': not a valid identifier\n");
		*equals = '=';
		return (1);
	}
	*equals = '=';
	len = equals - arg;
	var_name = malloc(len + 1);
	if (!var_name)
		return (1);
	ft_strncpy(var_name, arg, len);
	var_name[len] = '\0';
	ctx->info->env = set_env_var(ctx->info->env, var_name, equals + 1, 1);
	free(var_name);
	return (0);
}

static int	handle_var_without_value(char *arg, t_exec_ctx *ctx)
{
	char	*value;

	if (!is_valid_identifier(arg))
	{
		write_error_with_arg("bash: export: `", arg,
			"': not a valid identifier\n");
		return (1);
	}
	value = get_env_value(ctx->info->env, arg);
	if (value)
		ctx->info->env = set_env_var(ctx->info->env, arg, value, 1);
	return (0);
}

int	builtin_export(char **args, t_exec_ctx *ctx)
{
	int		ret;
	char	*equals;
	int		i;

	ret = 0;
	if (!args[1])
	{
		print_env_vars(ctx->info->env);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		equals = ft_strchr(args[i], '=');
		if (equals)
			ret |= handle_var_with_value(args[i], equals, ctx);
		else
			ret |= handle_var_without_value(args[i], ctx);
		i++;
	}
	return (ret);
}

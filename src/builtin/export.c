/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:56:26 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	builtin_export(char **args, t_exec_ctx *ctx)
{
	int			ret;
	char		*equals;
	char		*var_name;
	char		*value;
	int			i;

	ret = 0;
	if (!args[1])
	{
		i = 0;
		while (ctx->info->env[i])
		{
			printf("declare -x %s\n", ctx->info->env[i]);
			i++;
		}
		return (0);
	}
	for (i = 1; args[i]; i++)
	{
		equals = ft_strchr(args[i], '=');
		if (equals)
		{
			*equals = '\0';
			if (!is_valid_identifier(args[i]))
			{
				write_error_with_arg("bash: export: `", args[i], "': not a valid identifier\n");
				*equals = '=';
				ret = 1;
				continue ;
			}
			*equals = '=';
			var_name = malloc(equals - args[i] + 1);
			if (!var_name)
				return (1);
			ft_strncpy(var_name, args[i], equals - args[i]);
			var_name[equals - args[i]] = '\0';
			ctx->info->env = set_env_var(ctx->info->env, var_name, equals + 1, 1);
			free(var_name);
		}
		else
		{
			if (!is_valid_identifier(args[i]))
			{
				write_error_with_arg("bash: export: `", args[i], "': not a valid identifier\n");
				ret = 1;
				continue ;
			}
			value = get_env_value(ctx->info->env, args[i]);
			if (value)
				ctx->info->env = set_env_var(ctx->info->env, args[i], value, 1);
		}
	}
	return (ret);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:56:19 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static const t_builtin	*get_builtin_table(void)
{
	static const t_builtin	builtins[] = {{"cd", builtin_cd}, {"echo",
			builtin_echo}, {"pwd", builtin_pwd}, {"export", builtin_export},
			{"unset", builtin_unset}, {"env", builtin_env}, {"exit",
			builtin_exit}, {NULL, NULL}};

	return (builtins);
}

int	is_builtin(const char *cmd)
{
	const t_builtin	*builtins = get_builtin_table();

	for (int i = 0; builtins[i].name; i++)
	{
		if (ft_strcmp(cmd, builtins[i].name) == 0)
			return (1);
	}
	return (0);
}

int	execute_builtin(char **args, t_exec_ctx *ctx)
{
	const t_builtin	*builtins = get_builtin_table();
	int				status;

	for (int i = 0; builtins[i].name; i++)
	{
		if (ft_strcmp(args[0], builtins[i].name) == 0)
		{
			status = builtins[i].func(args, ctx);
			ctx->last_exit_status = status;
			return (status);
		}
	}
	return (127);
}

int	is_valid_identifier(const char *str)
{
	if (!str || !*str)
		return (0);
	if (!ft_isalpha((unsigned char)*str) && *str != '_')
		return (0);
	for (const char *p = str + 1; *p; p++)
	{
		if (!ft_isalnum((unsigned char)*p) && *p != '_')
			return (0);
	}
	return (1);
}

int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	while (str[i] == ' ' || str[i] == '\t')
		i++;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
		{
			while (str[i] == ' ' || str[i] == '\t')
				i++;
			return (str[i] == '\0');
		}
		i++;
	}
	return (1);
}

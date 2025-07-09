/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 11:44:52 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	builtin_echo(char **args, t_exec_ctx *ctx)
{
	int	i;
	int	newline;

	(void)ctx;
	i = 1;
	newline = 1;
	while (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		write(STDOUT_FILENO, args[i], ft_strlen(args[i]));
		if (args[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}

int	builtin_pwd(char **args, t_exec_ctx *ctx)
{
	char	cwd[1024];

	(void)ctx;
	if (args[1])
	{
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	else
	{
		perror("pwd");
		return (1);
	}
}

int	builtin_env(char **args, t_exec_ctx *ctx)
{
	int	i;

	(void)args;
	i = 0;
	while (ctx->info->env[i])
	{
		printf("%s\n", ctx->info->env[i]);
		i++;
	}
	return (0);
}

int	builtin_unset(char **args, t_exec_ctx *ctx)
{
	int	i;

	if (!args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			i++;
			continue ;
		}
		ctx->info->env = unset_env_var(ctx->info->env, args[i]);
		i++;
	}
	return (0);
}

int	builtin_exit(char **args, t_exec_ctx *ctx)
{
	int		exit_code;
	long	val;

	exit_code = ctx->last_exit_status;
	printf("exit\n");
	if (args[1] && args[2])
		write_error("bash: exit: too many arguments\n");
	if (args[1])
	{
		if (!is_valid_number(args[1]))
		{
			write_error_with_arg("bash: exit: ", args[1],
				": numeric argument required\n");
			exit_code = 2;
		}
		else
		{
			val = ft_atoi(args[1]);
			exit_code = ((val % 256) + 256) % 256;
		}
	}
	ctx->info->exit_f = 0;
	ctx->info->last_exit_status = exit_code;
	return (exit_code);
}

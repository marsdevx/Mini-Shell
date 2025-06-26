/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/26 15:57:03 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

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

static int	is_valid_identifier(const char *str)
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

int	builtin_cd(char **args, t_exec_ctx *ctx)
{
	char	*path;
	char	cwd[1024];

	(void)ctx;
	if (args[1] && args[2])
	{
		fprintf(stderr, "bash: cd: too many arguments\n");
		return (1);
	}
	if (!args[1])
	{
		path = getenv("HOME");
		if (!path)
		{
			fprintf(stderr, "bash: cd: HOME not set\n");
			return (1);
		}
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = getenv("OLDPWD");
		if (!path)
		{
			fprintf(stderr, "bash: cd: OLDPWD not set\n");
			return (1);
		}
		printf("%s\n", path);
	}
	else
	{
		path = args[1];
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		setenv("OLDPWD", cwd, 1);
	if (chdir(path) != 0)
	{
		fprintf(stderr, "bash: cd: %s: No such file or directory\n", path);
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		setenv("PWD", cwd, 1);
	return (0);
}

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
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	fflush(stdout);
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
	extern char	**environ;

	(void)args;
	(void)ctx;
	for (int i = 0; environ[i]; i++)
		printf("%s\n", environ[i]);
	return (0);
}

int	builtin_export(char **args, t_exec_ctx *ctx)
{
	int			ret;
	extern char	**environ;
	char		*equals;
	char		*var_name;
	char		*value;

	(void)ctx;
	ret = 0;
	if (!args[1])
	{
		for (int i = 0; environ[i]; i++)
			printf("declare -x %s\n", environ[i]);
		return (0);
	}
	for (int i = 1; args[i]; i++)
	{
		equals = ft_strchr(args[i], '=');
		if (equals)
		{
			*equals = '\0';
			if (!is_valid_identifier(args[i]))
			{
				fprintf(stderr, "bash: export: `%s': not a valid identifier\n",
					args[i]);
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
			setenv(var_name, equals + 1, 1);
			free(var_name);
		}
		else
		{
			if (!is_valid_identifier(args[i]))
			{
				fprintf(stderr, "bash: export: `%s': not a valid identifier\n",
					args[i]);
				ret = 1;
				continue ;
			}
			value = getenv(args[i]);
			if (value)
				setenv(args[i], value, 1);
		}
	}
	return (ret);
}

int ft_unsetenv(const char *name)
{
		extern char **environ;
    size_t  name_len;
    int     i, j, count;
    char  **new_env;

    if (!name || !*name)
        return 0;

    name_len = ft_strlen(name);
    for (count = 0; environ[count]; count++)
        ;

    new_env = malloc(sizeof(char *) * (count + 1));
    if (!new_env)
        return -1;

    for (i = 0, j = 0; environ[i]; i++)
    {
        if (ft_strncmp(environ[i], name, name_len) == 0
            && environ[i][name_len] == '=')
        {
            free(environ[i]);
        }
        else
        {
            new_env[j++] = environ[i];
        }
    }
    new_env[j] = NULL;

    environ = new_env;
    return 0;
}

int	builtin_unset(char **args, t_exec_ctx *ctx)
{
	(void)ctx;
	if (!args[1])
		return (0);
	for (int i = 1; args[i]; i++)
		ft_unsetenv(args[i]);
	return (0);
}

int	builtin_exit(char **args, t_exec_ctx *ctx)
{
	int exit_code = ctx->last_exit_status;

	printf("exit\n");

	if (args[1] && args[2])
		fprintf(stderr, "bash: exit: too many arguments\n");

	if (args[1])
	{
		char *endptr;
		long val = ft_strtol(args[1], &endptr, 10);

		if (*endptr != '\0')
		{
			fprintf(stderr, "bash: exit: %s: numeric argument required\n",
				args[1]);
			exit_code = 2;
		}
		else
		{
			exit_code = ((val % 256) + 256) % 256;
		}
	}

	ctx->info->exit_f = 0;
	ctx->info->last_exit_status = exit_code;

	return (exit_code);
}

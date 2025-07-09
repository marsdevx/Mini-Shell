/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 21:04:11 by dkot             ###   ########.fr       */
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

static int	prepare_command(t_group *grp, char ***argv)
{
	if (!grp || !grp->argv)
		return (0);
	*argv = group_to_argv(grp);
	if (!*argv)
		return (1);
	if (!(*argv)[0])
	{
		free_argv(*argv);
		return (0);
	}
	return (-1);
}

static int	execute_command(char **argv, t_exec_ctx *ctx)
{
	int	status;

	if (!argv[0] || ft_strlen(argv[0]) == 0)
		status = 0;
	else if (is_builtin(argv[0]))
		status = execute_builtin(argv, ctx);
	else
		status = execute_external(argv, ctx);
	return (status);
}

int	execute_single_command(t_group *grp, t_exec_ctx *ctx)
{
	char	**argv;
	int		status;
	int		stdin_temp;
	int		stdout_temp;
	int		prep_result;

	prep_result = prepare_command(grp, &argv);
	if (prep_result >= 0)
		return (prep_result);
	stdin_temp = dup(STDIN_FILENO);
	stdout_temp = dup(STDOUT_FILENO);
	if (setup_redirections(&argv) < 0)
	{
		cleanup(stdin_temp, stdout_temp, argv);
		return (1);
	}
	status = execute_command(argv, ctx);
	cleanup(stdin_temp, stdout_temp, argv);
	return (status);
}

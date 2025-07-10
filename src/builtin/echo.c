/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 16:10:16 by dkot              #+#    #+#             */
/*   Updated: 2025/07/10 16:11:42 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	check_n_flag(char **args, int *i)
{
	int	j;
	int	newline;

	newline = 1;
	while (args[*i] && args[*i][0] == '-' && args[*i][1] == 'n')
	{
		j = 1;
		while (args[*i][j] == 'n')
			j++;
		if (args[*i][j] == '\0')
		{
			newline = 0;
			(*i)++;
		}
		else
			break ;
	}
	return (newline);
}

int	builtin_echo(char **args, t_exec_ctx *ctx)
{
	int	i;
	int	newline;

	(void)ctx;
	i = 1;
	newline = check_n_flag(args, &i);
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

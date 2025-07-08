/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 22:43:21 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	count_args(t_list *args)
{
	int	count;

	count = 0;
	while (args)
	{
		count++;
		args = args->next;
	}
	return (count);
}

void	free_argv(char **argv)
{
	if (!argv)
		return ;
	for (int i = 0; argv[i]; i++)
		free(argv[i]);
	free(argv);
}

void cleanup(int stdin_temp, int stdout_temp, char **argv)
{
	dup2(stdin_temp, STDIN_FILENO);
	dup2(stdout_temp, STDOUT_FILENO);
	close(stdin_temp);
	close(stdout_temp);
	free_argv(argv);
}

void	remove_argv_element(char **argv, int index)
{
	free(argv[index]);
	while (argv[index])
	{
		argv[index] = argv[index + 1];
		index++;
	}
}
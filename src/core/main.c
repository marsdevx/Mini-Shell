/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 20:27:51 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/18 22:14:01 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

int	main(void)
{
	char	*input;

	while (1)
	{
		input = readline(PROMPT);
		if (!input)
		{
			printf("\n");
			break ;
		}
		if (ft_strncmp(input, EXIT_CMD, sizeof(EXIT_CMD)) == 0)
		{
			printf("exit\n");
			free(input);
			break ;
		}
		if (*input)
		{
			add_history(input);
			printf("minishell: %s: command not found\n", input);
		}
		free(input);
	}
	clear_history();
	return (0);
}

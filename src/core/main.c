/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 00:30:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/19 00:12:39 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

int main(int argc, char **argv, char **envp) {
  char *input;

  (void)argc;  // Unused parameter
  (void)argv;  // Unused parameter

  while (1) {
    input = readline(PROMPT);
    if (!input) {
      printf("\n");
      break;
    }

    if (*input) {
      add_history(input);
      execute_command(input, envp);  // Pass envp to execute_command
    }

    free(input);
  }

  clear_history();
  return 0;
}

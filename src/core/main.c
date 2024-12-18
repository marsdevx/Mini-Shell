/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 20:27:51 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/18 21:06:31 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

#define PROMPT "minishell> "
#define EXIT_CMD "exit"

int main(void) {
  char *input;

  while (1) {
    // Display the prompt and read input using readline
    input = readline(PROMPT);

    // Exit the loop if EOF (Ctrl+D) or if input is NULL
    if (!input) {
      printf("\n");
      break;
    }

    // If the input is "exit", exit the shell
    if (strncmp(input, EXIT_CMD, sizeof(EXIT_CMD)) == 0) {
      printf("exit\n");
      free(input);
      break;
    }

    // Handle empty input (do nothing)
    if (*input) {
      // Add the command to history
      add_history(input);
      // Command not found for any other input
      printf("minishell: %s: command not found\n", input);
    }

    // Free the input buffer
    free(input);
  }

  // Clear the history before exiting
  clear_history();
  return 0;
}

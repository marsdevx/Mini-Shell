/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 20:27:51 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/18 22:44:12 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

void cmd_pwd(void) {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
    printf("%s\n", cwd);
  else
    perror("minishell: pwd");
}

void cmd_exit(void) {
  printf("exit\n");
  exit(0);
}

void execute_command(char *input) {
  t_command commands[] = {
    {PWD_CMD, cmd_pwd},
    {EXIT_CMD, cmd_exit},
    {NULL, NULL}
  };

  for (int i = 0; commands[i].name != NULL; i++) {
    if (strcmp(input, commands[i].name) == 0) {
      commands[i].func();
      return;
    }
  }

  printf("minishell: %s: command not found\n", input);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 20:27:51 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/18 23:11:30 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

// pwd command: print the current working directory
void cmd_pwd(void) {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
    printf("%s\n", cwd);
  else
    perror("minishell: pwd");
}

// exit command: exit the shell
void cmd_exit(void) {
  printf("exit\n");
  exit(0);
}

// env command: print environment variables
void cmd_env(char **envp) {
  for (int i = 0; envp[i] != NULL; i++) {
    printf("%s\n", envp[i]);
  }
}

// Execute the appropriate command
void execute_command(char *input, char **envp) {
  char *args = strdup(input); // Duplicate input to tokenize
  char *command = strtok(args, " ");
  char *arg = strtok(NULL, " "); // Get the argument for the command

  // Command struct array
  t_command commands[] = {
    {PWD_CMD, (void (*)(void))cmd_pwd},
    {EXIT_CMD, (void (*)(void))cmd_exit},
    {ENV_CMD, (void (*)(void))cmd_env},
    {"cd", (void (*)(void))cmd_cd},
    {NULL, NULL}
  };

  // Iterate through commands and execute the matching one
  for (int i = 0; commands[i].name != NULL; i++) {
    if (strcmp(command, commands[i].name) == 0) {
      if (strcmp(command, "cd") == 0)
        cmd_cd(arg);
      else if (strcmp(command, "env") == 0)
        cmd_env(envp);
      else
        commands[i].func();

      free(args);
      return;
    }
  }

  // If no command matches
  printf("minishell: %s: command not found\n", command);
  free(args);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 01:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/18 23:33:03 by marksylaiev      ###   ########.fr       */
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

// unset command: unset environment variables
void cmd_unset(char *arg, char **envp) {
  if (!arg) {
    fprintf(stderr, "minishell: unset: missing argument\n");
    return;
  }

  int i = 0;
  while (envp[i] != NULL) {
    if (strncmp(envp[i], arg, strlen(arg)) == 0 && envp[i][strlen(arg)] == '=') {
      // Shift the remaining entries to remove the variable
      int j = i;
      while (envp[j] != NULL) {
        envp[j] = envp[j + 1];
        j++;
      }
      return;
    }
    i++;
  }

  fprintf(stderr, "minishell: unset: %s not found\n", arg);
}

// export command: print sorted environment variables
void cmd_export(char **envp) {
  int i, j;
  char *temp;

  // Count the number of environment variables
  int count = 0;
  while (envp[count] != NULL)
    count++;

  // Create a copy of the environment array for sorting
  char **sorted_env = malloc((count + 1) * sizeof(char *));
  if (!sorted_env) {
    perror("minishell: export");
    return;
  }

  for (i = 0; i < count; i++)
    sorted_env[i] = envp[i];
  sorted_env[count] = NULL;

  // Simple bubble sort to sort the environment variables
  for (i = 0; i < count - 1; i++) {
    for (j = 0; j < count - i - 1; j++) {
      if (strcmp(sorted_env[j], sorted_env[j + 1]) > 0) {
        temp = sorted_env[j];
        sorted_env[j] = sorted_env[j + 1];
        sorted_env[j + 1] = temp;
      }
    }
  }

  // Print the sorted environment variables
  for (i = 0; sorted_env[i] != NULL; i++) {
    printf("declare -x %s\n", sorted_env[i]);
  }

  free(sorted_env);
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
    {"unset", (void (*)(void))cmd_unset},
    {"export", (void (*)(void))cmd_export},
    {NULL, NULL}
  };

  // Iterate through commands and execute the matching one
  for (int i = 0; commands[i].name != NULL; i++) {
    if (strcmp(command, commands[i].name) == 0) {
      if (strcmp(command, "cd") == 0)
        cmd_cd(arg);
      else if (strcmp(command, "unset") == 0)
        cmd_unset(arg, envp);
      else if (strcmp(command, "env") == 0)
        cmd_env(envp);
      else if (strcmp(command, "export") == 0)
        cmd_export(envp);
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
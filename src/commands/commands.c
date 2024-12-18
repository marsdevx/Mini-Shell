/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 01:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/18 23:58:17 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

#include "../header.h"

void cmd_echo(char *args) {
  int my_newline = 1;
  if (args && strncmp(args, "-n", 2) == 0 && (args[2] == ' ' || args[2] == '\0')) {
    my_newline = 0;
    args = args + 2;
    while (*args == ' ') args++;
  }
  if (args)
    printf("%s", args);
  if (my_newline)
    printf("\n");
}

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

void cmd_env(char **envp) {
  for (int i = 0; envp[i] != NULL; i++) {
    printf("%s\n", envp[i]);
  }
}

void cmd_unset(char *arg, char **envp) {
  if (!arg) {
    fprintf(stderr, "minishell: unset: missing argument\n");
    return;
  }
  int i = 0;
  while (envp[i] != NULL) {
    if (strncmp(envp[i], arg, strlen(arg)) == 0 && envp[i][strlen(arg)] == '=') {
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

void cmd_export(char **envp) {
  int i, j;
  char *temp;
  int count = 0;
  while (envp[count] != NULL)
    count++;
  char **sorted_env = malloc((count + 1) * sizeof(char *));
  if (!sorted_env) {
    perror("minishell: export");
    return;
  }
  for (i = 0; i < count; i++)
    sorted_env[i] = envp[i];
  sorted_env[count] = NULL;
  for (i = 0; i < count - 1; i++) {
    for (j = 0; j < count - i - 1; j++) {
      if (strcmp(sorted_env[j], sorted_env[j + 1]) > 0) {
        temp = sorted_env[j];
        sorted_env[j] = sorted_env[j + 1];
        sorted_env[j + 1] = temp;
      }
    }
  }
  for (i = 0; sorted_env[i] != NULL; i++) {
    printf("declare -x %s\n", sorted_env[i]);
  }
  free(sorted_env);
}

void execute_command(char *input, char **envp) {
  char *args = strdup(input);
  char *command = strtok(args, " ");
  char *arg = strtok(NULL, "");
  t_command commands[] = {
    {PWD_CMD, (void (*)(void))cmd_pwd},
    {EXIT_CMD, (void (*)(void))cmd_exit},
    {ENV_CMD, (void (*)(void))cmd_env},
    {"cd", (void (*)(void))cmd_cd},
    {"unset", (void (*)(void))cmd_unset},
    {"export", (void (*)(void))cmd_export},
    {"echo", (void (*)(void))cmd_echo},
    {NULL, NULL}
  };
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
      else if (strcmp(command, "echo") == 0)
        cmd_echo(arg);
      else
        commands[i].func();
      free(args);
      return;
    }
  }
  printf("minishell: %s: command not found\n", command);
  free(args);
}

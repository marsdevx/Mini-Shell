/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 01:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/19 03:14:23 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

void init_envp(char **envp) {
  int i = 0;
  while (envp[i])
    i++;

  g_envp = malloc((i + 1) * sizeof(char *));
  if (!g_envp) {
    perror("minishell: malloc");
    exit(EXIT_FAILURE);
  }

  for (int j = 0; j < i; j++)
    g_envp[j] = strdup(envp[j]);
  g_envp[i] = NULL;
}

char *expand_home(char *path) {
  char *home = getenv("HOME");

  if (!home) {
    fprintf(stderr, "minishell: cd: HOME not set\n");
    return NULL;
  }

  if (strcmp(path, "~") == 0) {
    return strdup(home);
  } else if (strncmp(path, "~/", 2) == 0) {
    char *expanded_path = malloc(strlen(home) + strlen(path));
    if (!expanded_path) {
      perror("minishell: cd");
      return NULL;
    }
    sprintf(expanded_path, "%s%s", home, path + 1);
    return expanded_path;
  }

  return strdup(path);
}

void cmd_cd(char *arg) {
  char *path;

  if (!arg) {
    path = getenv("HOME");
    if (!path) {
      fprintf(stderr, "minishell: cd: HOME not set\n");
      return;
    }
    if (chdir(path) != 0)
      perror("minishell: cd");
    return;
  }

  path = expand_home(arg);
  if (!path)
    return;

  if (chdir(path) != 0)
    perror("minishell: cd");

  free(path);
}

void cmd_echo(char *args, char **envp) {
  int my_newline = 1;
  int in_single_quote = 0;
  int in_double_quote = 0;
  char *result = malloc(1024);
  if (!result) {
    perror("minishell: malloc");
    return;
  }

	(void)envp;
  int i = 0, j = 0;

  if (args && strncmp(args, "-n", 2) == 0 && (args[2] == ' ' || args[2] == '\0')) {
    my_newline = 0;
    args += 2;
    while (*args == ' ') args++;
  }

  while (args && args[i]) {
    if (args[i] == '\'' && !in_double_quote) {
      in_single_quote = !in_single_quote;
      i++;
    } else if (args[i] == '"' && !in_single_quote) {
      in_double_quote = !in_double_quote;
      i++;
    } else if (args[i] == '$' && !in_single_quote) {
      i++;
      int var_start = i;
      while (isalnum(args[i]) || args[i] == '_')
        i++;
      char *var_name = strndup(&args[var_start], i - var_start);
      if (!var_name) {
        perror("minishell: strndup");
        free(result);
        return;
      }

      char *var_value = NULL;
      for (int k = 0; g_envp[k] != NULL; k++) {
        if (strncmp(g_envp[k], var_name, strlen(var_name)) == 0 && g_envp[k][strlen(var_name)] == '=') {
          var_value = &g_envp[k][strlen(var_name) + 1];
          break;
        }
      }

      if (var_value) {
        while (*var_value)
          result[j++] = *var_value++;
      }

      free(var_name);
    } else {
      result[j++] = args[i++];
    }
  }

  result[j] = '\0';
  printf("%s", result);
  if (my_newline)
    printf("\n");

  free(result);
}

void cmd_unset(char *arg) {
  if (!arg) {
    fprintf(stderr, "minishell: unset: missing argument\n");
    return;
  }

  // Remove surrounding quotes if they exist
  int len = strlen(arg);
  if ((arg[0] == '\'' && arg[len - 1] == '\'') || (arg[0] == '"' && arg[len - 1] == '"')) {
    arg[len - 1] = '\0';  // Remove the closing quote
    arg++;                // Skip the opening quote
  }

  // Check if the variable name is valid (letters, digits, underscores)
  for (int i = 0; arg[i]; i++) {
    if (!(isalnum(arg[i]) || arg[i] == '_')) {
      fprintf(stderr, "minishell: unset: `%s': not a valid identifier\n", arg);
      return;
    }
  }

  // Search for the variable and remove it
  int i = 0;
  while (g_envp[i] != NULL) {
    if (strncmp(g_envp[i], arg, strlen(arg)) == 0 && g_envp[i][strlen(arg)] == '=') {
      free(g_envp[i]);
      while (g_envp[i] != NULL) {
        g_envp[i] = g_envp[i + 1];  // Shift the remaining variables left
        i++;
      }
      return;  // Successfully unset the variable
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
        cmd_unset(arg);
      else if (strcmp(command, "env") == 0)
        cmd_env(envp);
      else if (strcmp(command, "export") == 0)
        cmd_export(envp);
			else if (strcmp(command, "echo") == 0)
				cmd_echo(arg, envp);
      else
        commands[i].func();
      free(args);
      return;
    }
  }
  printf("minishell: %s: command not found\n", command);
  free(args);
}

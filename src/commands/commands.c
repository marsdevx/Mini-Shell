/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 01:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/19 07:09:47 by marksylaiev      ###   ########.fr       */
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

void cmd_env(char **envp) {
  for (int i = 0; envp[i] != NULL; i++) {
    printf("%s\n", envp[i]);
  }
}

void cmd_cd(char *arg) {
  if (!arg) {
    char *home = getenv("HOME");
    if (!home) {
      fprintf(stderr, "minishell: cd: HOME not set\n");
      return;
    }
    if (chdir(home) != 0)
      perror("minishell: cd");
    return;
  }

  int len = strlen(arg);
  if ((arg[0] == '\'' && arg[len - 1] != '\'') || (arg[0] == '"' && arg[len - 1] != '"')) {
    fprintf(stderr, "minishell: error: unclosed quotes\n");
    return;
  }

  char *processed_path = NULL;

  if (arg[0] == '\'' && arg[len - 1] == '\'') {
    arg[len - 1] = '\0';
    processed_path = strdup(arg + 1);
  } else {
    if ((arg[0] == '"' && arg[len - 1] == '"')) {
      arg[len - 1] = '\0';
      arg++;
    }

    processed_path = expand_dollar(arg, g_envp);
    if (!processed_path) {
      fprintf(stderr, "minishell: cd: failed to expand path\n");
      return;
    }
  }

  char *final_path = expand_home(processed_path);
  free(processed_path);

  if (!final_path) {
    return;
  }

  if (chdir(final_path) != 0) {
    fprintf(stderr, "minishell: cd: %s: No such file or directory\n", final_path);
  }

  free(final_path);
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

  if (in_single_quote || in_double_quote) {
    fprintf(stderr, "minishell: error: unclosed quotes\n");
    free(result);
    return;
  }

  result[j] = '\0';
  printf("%s", result);
  if (my_newline)
    printf("\n");

  free(result);
}

void cmd_unset(char *arg) {
  if (!arg) {
    fprintf(stderr, "minishell: error: var not exist\n");
    return;
  }

  int len = strlen(arg);
  if ((arg[0] == '\'' && arg[len - 1] != '\'') || (arg[0] == '"' && arg[len - 1] != '"')) {
    fprintf(stderr, "minishell: error: unclosed quotes\n");
    return;
  }

  if ((arg[0] == '\'' && arg[len - 1] == '\'') || (arg[0] == '"' && arg[len - 1] == '"')) {
    arg[len - 1] = '\0';
    arg++;
  }

  for (int i = 0; arg[i]; i++) {
    if (!(isalnum(arg[i]) || arg[i] == '_')) {
      fprintf(stderr, "minishell: unset: `%s': not a valid identifier\n", arg);
      return;
    }
  }

  int i = 0;
  while (g_envp[i] != NULL) {
    if (strncmp(g_envp[i], arg, strlen(arg)) == 0 && g_envp[i][strlen(arg)] == '=') {
      free(g_envp[i]);
      while (g_envp[i] != NULL) {
        g_envp[i] = g_envp[i + 1];
        i++;
      }
      return;
    }
    i++;
  }

  fprintf(stderr, "minishell: error: var not exist\n");
}

void cmd_export(char **args) {
  if (!args || !args[0]) {
    int count = 0;
    while (g_envp[count] != NULL)
      count++;

    char **sorted_env = malloc((count + 1) * sizeof(char *));
    if (!sorted_env) {
      perror("minishell: export");
      return;
    }

    for (int i = 0; i < count; i++)
      sorted_env[i] = g_envp[i];
    sorted_env[count] = NULL;

    for (int i = 0; i < count - 1; i++) {
      for (int j = 0; j < count - i - 1; j++) {
        if (strcmp(sorted_env[j], sorted_env[j + 1]) > 0) {
          char *temp = sorted_env[j];
          sorted_env[j] = sorted_env[j + 1];
          sorted_env[j + 1] = temp;
        }
      }
    }

    for (int i = 0; sorted_env[i] != NULL; i++)
      printf("declare -x %s\n", sorted_env[i]);

    free(sorted_env);
    return;
  }

  for (int i = 0; args[i]; i++) {
    char *arg = args[i];

    int in_single_quote = 0;
    int in_double_quote = 0;
    for (int j = 0; arg[j]; j++) {
      if (arg[j] == '\'' && !in_double_quote)
        in_single_quote = !in_single_quote;
      else if (arg[j] == '"' && !in_single_quote)
        in_double_quote = !in_double_quote;
    }

    if (in_single_quote || in_double_quote) {
      fprintf(stderr, "minishell: export: error: unclosed quotes\n");
      continue;
    }

    char *equal_sign = strchr(arg, '=');
    if (!equal_sign) {
      fprintf(stderr, "minishell: export: invalid format\n");
      continue;
    }

    *equal_sign = '\0';
    char *var_name = arg;
    char *var_value = equal_sign + 1;

    int value_len = strlen(var_value);
    char *processed_value = NULL;

    if ((var_value[0] == '\'' && var_value[value_len - 1] == '\'') ||
        (var_value[0] == '"' && var_value[value_len - 1] == '"')) {
      var_value[value_len - 1] = '\0';
      processed_value = strdup(var_value + 1);
    } else {
      processed_value = strdup(var_value);
    }

    if (!processed_value) {
      perror("minishell: strdup");
      return;
    }

    if (var_value[0] == '"') {
      char *expanded_value = expand_dollar(processed_value, g_envp);
      free(processed_value);
      processed_value = expanded_value;
    }

    char *new_entry = malloc(strlen(var_name) + strlen(processed_value) + 2);
    if (!new_entry) {
      perror("minishell: malloc");
      free(processed_value);
      return;
    }
    sprintf(new_entry, "%s=%s", var_name, processed_value);
    free(processed_value);

    int j = 0;
    for (; g_envp[j] != NULL; j++) {
      if (strncmp(g_envp[j], var_name, strlen(var_name)) == 0 && g_envp[j][strlen(var_name)] == '=') {
        free(g_envp[j]);
        g_envp[j] = new_entry;
        return;
      }
    }

    g_envp[j] = new_entry;
    g_envp[j + 1] = NULL;
  }
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
        cmd_export(arg ? (char *[]){arg, NULL} : NULL);
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

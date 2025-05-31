/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 01:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2025/05/28 22:44:31 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

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
  int i = 0;

  if (!args) {
    printf("\n");
    return;
  }

  // Check for the "-n" option
  if (strncmp(args, "-n", 2) == 0 && (args[2] == ' ' || args[2] == '\0')) {
    my_newline = 0;
    args += 2;
    while (*args == ' ') args++;
  }

  int in_single_quote = 0;
  int in_double_quote = 0;

  while (args[i]) {
    // Handle single quotes
    if (args[i] == '\'' && !in_double_quote) {
      in_single_quote = !in_single_quote;
      i++;
    }
    // Handle double quotes
    else if (args[i] == '"' && !in_single_quote) {
      in_double_quote = !in_double_quote;
      i++;
    }
    // Print characters as they are
    else {
      putchar(args[i]);
      i++;
    }
  }

  if (my_newline) {
    putchar('\n');
  }

  (void)envp;  // Suppress unused parameter warning
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

void handle_redirection(char **cmd_parts) {
  int i = 0;

  while (cmd_parts[i]) {
    // Input redirection: <
    if (strcmp(cmd_parts[i], "<") == 0) {
      int fd = open(cmd_parts[i + 1], O_RDONLY);
      if (fd < 0) {
        perror("minishell: open");
        return;
      }
      dup2(fd, STDIN_FILENO);
      close(fd);

      // Remove the redirection operator and filename from the arguments
      cmd_parts[i] = NULL;
      cmd_parts[i + 1] = NULL;
    }
    // Output redirection: >
    else if (strcmp(cmd_parts[i], ">") == 0) {
      int fd = open(cmd_parts[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
        perror("minishell: open");
        return;
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);

      cmd_parts[i] = NULL;
      cmd_parts[i + 1] = NULL;
    }
    // Append redirection: >>
    else if (strcmp(cmd_parts[i], ">>") == 0) {
      int fd = open(cmd_parts[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
      if (fd < 0) {
        perror("minishell: open");
        return;
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);

      cmd_parts[i] = NULL;
      cmd_parts[i + 1] = NULL;
    }
    i++;
  }
}

// Function to split input while preserving quoted strings
void split_input(char *input, char *cmd_parts[], int *count) {
  int i = 0, j = 0;
  int in_single_quote = 0, in_double_quote = 0;
  char buffer[1024] = {0};

  while (input[i]) {
    if (input[i] == '\'' && !in_double_quote) {
      in_single_quote = !in_single_quote;
    } else if (input[i] == '"' && !in_single_quote) {
      in_double_quote = !in_double_quote;
    } else if (input[i] == ' ' && !in_single_quote && !in_double_quote) {
      if (j > 0) {
        buffer[j] = '\0';
        cmd_parts[(*count)++] = strdup(buffer);
        j = 0;
      }
    } else {
      buffer[j++] = input[i];
    }
    i++;
  }

  if (j > 0) {
    buffer[j] = '\0';
    cmd_parts[(*count)++] = strdup(buffer);
  }
  cmd_parts[*count] = NULL;
}

void execute_command(char *input, char **envp) {
  char *cmd_parts[256] = {0};
  int count = 0;

  // Split input while handling quoted strings
  split_input(input, cmd_parts, &count);

  if (count == 0) {
    return;
  }

  // Backup file descriptors for restoring later
  int stdout_backup = dup(STDOUT_FILENO);
  int stdin_backup = dup(STDIN_FILENO);

  // Handle redirection before executing the command
  handle_redirection(cmd_parts);

  // Rebuild the command parts after handling redirection
  char *filtered_cmd[256] = {0};
  int j = 0;
  for (int i = 0; i < count; i++) {
    if (cmd_parts[i]) {
      filtered_cmd[j++] = cmd_parts[i];
    }
  }
  filtered_cmd[j] = NULL;

  // Execute built-in commands
  if (filtered_cmd[0]) {
    if (strcmp(filtered_cmd[0], "pwd") == 0) {
      cmd_pwd();
    } else if (strcmp(filtered_cmd[0], "exit") == 0) {
      cmd_exit();
    } else if (strcmp(filtered_cmd[0], "env") == 0) {
      cmd_env(envp);
    } else if (strcmp(filtered_cmd[0], "cd") == 0) {
      cmd_cd(filtered_cmd[1]);
    } else if (strcmp(filtered_cmd[0], "unset") == 0) {
      cmd_unset(filtered_cmd[1]);
    } else if (strcmp(filtered_cmd[0], "export") == 0) {
      cmd_export(&filtered_cmd[1]);
    } else if (strcmp(filtered_cmd[0], "echo") == 0) {
      if (filtered_cmd[1]) {
        cmd_echo(filtered_cmd[1], envp);
      } else {
        // Read from stdin if no arguments are provided due to input redirection
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
          buffer[bytes_read] = '\0';
          printf("%s", buffer);
        }
      }
    } else {
      // Execute external command using execve
      if (execve(filtered_cmd[0], filtered_cmd, envp) == -1) {
        perror("minishell: execve");
      }
    }
  }

  // Restore original file descriptors
  dup2(stdout_backup, STDOUT_FILENO);
  dup2(stdin_backup, STDIN_FILENO);
  close(stdout_backup);
  close(stdin_backup);

  // Free allocated memory for command parts
  for (int i = 0; cmd_parts[i]; i++) {
    free(cmd_parts[i]);
  }
}

#include "../header.h"

void cmd_export(char **args) {
  if (!args || !args[0]) {
    // List all environment variables in alphabetical order
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

    // Sort the environment variables alphabetically
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

    // Check for unclosed quotes in the entire argument
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

    // Split the argument into the variable name and value
    char *equal_sign = strchr(arg, '=');
    if (!equal_sign) {
      fprintf(stderr, "minishell: export: invalid format\n");
      continue;
    }

    *equal_sign = '\0';
    char *var_name = arg;
    char *var_value = equal_sign + 1;

    // Handle surrounding quotes in the value
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

    // Expand variables if the value is enclosed in double quotes
    if (var_value[0] == '"') {
      char *expanded_value = expand_dollar(processed_value, g_envp);
      free(processed_value);
      processed_value = expanded_value;
    }

    // Construct the new environment variable string
    char *new_entry = malloc(strlen(var_name) + strlen(processed_value) + 2);
    if (!new_entry) {
      perror("minishell: malloc");
      free(processed_value);
      return;
    }
    sprintf(new_entry, "%s=%s", var_name, processed_value);
    free(processed_value);

    // Add or update the variable in g_envp
    int j = 0;
    for (; g_envp[j] != NULL; j++) {
      if (strncmp(g_envp[j], var_name, strlen(var_name)) == 0 && g_envp[j][strlen(var_name)] == '=') {
        free(g_envp[j]);
        g_envp[j] = new_entry;
        return;
      }
    }

    // Add the new variable if it doesn't exist
    g_envp[j] = new_entry;
    g_envp[j + 1] = NULL;
  }
}
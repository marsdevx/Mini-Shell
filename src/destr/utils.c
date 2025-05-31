/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 01:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2025/05/28 22:44:20 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

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
      perror("minishell: malloc");
      return NULL;
    }
    sprintf(expanded_path, "%s%s", home, path + 1);
    return expanded_path;
  }

  return strdup(path);
}

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

char *expand_dollar(char *input, char **envp) {
  char *result = malloc(1024);
  if (!result) {
    perror("minishell: malloc");
    return NULL;
  }

  int i = 0, j = 0;
  while (input[i]) {
    if (input[i] == '$' && (ft_isalnum(input[i + 1]) || input[i + 1] == '_')) {
      i++;
      int var_start = i;
      while (ft_isalnum(input[i]) || input[i] == '_')
        i++;
      char *var_name = strndup(&input[var_start], i - var_start);
      if (!var_name) {
        perror("minishell: strndup");
        free(result);
        return NULL;
      }

      char *var_value = NULL;
      for (int k = 0; envp[k] != NULL; k++) {
        if (strncmp(envp[k], var_name, strlen(var_name)) == 0 && envp[k][strlen(var_name)] == '=') {
          var_value = &envp[k][strlen(var_name) + 1];
          break;
        }
      }

      if (var_value) {
        for (int k = 0; var_value[k]; k++)
          result[j++] = var_value[k];
      }

      free(var_name);
    } else {
      result[j++] = input[i++];
    }
  }
  result[j] = '\0';
  return result;
}

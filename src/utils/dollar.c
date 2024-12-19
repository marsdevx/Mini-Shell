/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 01:00:00 by marksylaiev       #+#    #+#             */
/*   Updated: 2024/12/19 07:03:45 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

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

#include "../header.h"

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
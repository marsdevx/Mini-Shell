#include "../header.h"

// Helper function to expand ~ to the HOME directory
char *expand_home(char *path) {
  char *home = getenv("HOME");

  if (!home) {
    fprintf(stderr, "minishell: cd: HOME not set\n");
    return NULL;
  }

  // If the path is exactly "~" or starts with "~/"
  if (strcmp(path, "~") == 0) {
    return strdup(home);
  } else if (strncmp(path, "~/", 2) == 0) {
    // Allocate space for the expanded path
    char *expanded_path = malloc(strlen(home) + strlen(path));
    if (!expanded_path) {
      perror("minishell: cd");
      return NULL;
    }
    sprintf(expanded_path, "%s%s", home, path + 1);
    return expanded_path;
  }

  // If no ~ is present, return a copy of the original path
  return strdup(path);
}

// cd command: change the current working directory
void cmd_cd(char *arg) {
  char *path;

  // Handle no argument case (default to HOME)
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

  // Expand ~ if necessary
  path = expand_home(arg);
  if (!path)
    return;

  // Change to the specified directory
  if (chdir(path) != 0)
    perror("minishell: cd");

  free(path);
}
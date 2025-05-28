#include "header.h"

char **g_envp = NULL;

int main(int argc, char **argv, char **envp) {
  init_envp(envp);

  (void)argv;
  (void)argc;

  char *line;
  while (1) {
    line = readline("minishell> ");
    if (!line)
      break;

    add_history(line);
    execute_command(line, g_envp);
    free(line);
  }

  for (int i = 0; g_envp[i]; i++)
    free(g_envp[i]);
  free(g_envp);

  return 0;
}

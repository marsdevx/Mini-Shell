#include "../header.h"

char **g_envp = NULL;  // Define the global variable

int main(int argc, char **argv, char **envp) {
  init_envp(envp);  // Initialize the global environment copy

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

  // Free g_envp before exiting
  for (int i = 0; g_envp[i]; i++)
    free(g_envp[i]);
  free(g_envp);

  return 0;
}
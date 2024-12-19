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

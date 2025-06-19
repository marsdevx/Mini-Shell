#include "../init/header.h"

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

int run_builtin(const char *cmd) {
  if (!cmd)
    return 0;

  if (strcmp(cmd, "pwd") == 0) {
    cmd_pwd();
    return 1;
  } else if (strcmp(cmd, "exit") == 0) {
    cmd_exit();
    return 1;
  }

  return 0;
}
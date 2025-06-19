#include "../init/header.h"

void process_input(char *line, t_info *info, char **envp)
{
  if (!line)
    return;

  t_list *tokens = lexer(line);
  if (!tokens)
    return;

  t_list *groups = parser(tokens);
  if (groups)
  {
    execute_commands(groups, envp, info);
    free_groups(&groups);
  }

  ft_free_tokens(&tokens);
}

int main(int argc, char *argv[], char *envp[])
{
    t_info info;
    char *line;

    (void)argc;
    (void)argv;

    if (ft_init(&info) != 0) {
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);

    while (info.exit_f) {
        line = ft_readline("minishell> ");
        if (line == NULL) {
            printf("exit\n");
            break;
        }
        if (*line) {
            process_input(line, &info, envp);
        }
        free(line);
    }

    return info.exit_f;
}
#include "../header/header.h"

int main(int argc, char *argv[], char *envp[])
{
    t_info info;
    char *line;

    (void)argc;
    (void)argv;
    (void)envp;

    if (ft_init(&info) != 0) {
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);

    while (1) {
        line = ft_readline("minishell> ");
        if (line == NULL) {
            printf("exit\n");
            break;
        }
        if (*line) {
            process_input(line, &info);
        }
        free(line);
    }

    return 0;
}
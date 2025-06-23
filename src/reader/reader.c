#include "../init/header.h"

char *ft_readline(const char *prompt)
{
    char *line = readline(prompt);
    if (line && *line) {
        add_history(line);
    }
    return line;
}

int ft_init(t_info *info)
{
    info->exit_f = 1;
    info->last_exit_status = 0;
    return 0;
}

void handle_sigint(int sig)
{
    if (sig == SIGINT) {
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}
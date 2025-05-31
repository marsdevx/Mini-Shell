
#include "../header/header.h"


static char *ft_readline(const char *prompt)
{
    char *line = readline(prompt);
    if (line && *line) {
        add_history(line);
    }
    return line;
}

static int ft_init(t_info *info)
{
    info->exit_f = 1;
    return 0;
}

static void handle_sigint(int sig)
{
    if (sig == SIGINT) {
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}
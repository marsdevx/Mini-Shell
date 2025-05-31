#ifndef READER_H
# define READER_H

#  include "../init/init.h"


static int ft_init(t_info *info);
static void handle_sigint(int sig);
static char *ft_readline(const char *prompt);


#endif
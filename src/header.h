#ifndef HEADER_H
# define HEADER_H

/* For printf and FILE type (must be included first) */
# include <stdio.h>

/* For readline, rl_clear_history, rl_on_new_line, rl_replace_line,
   rl_redisplay, add_history */
# include <readline/readline.h>
# include <readline/history.h>

/* For libft */
# include "../libs/libft/libft.h"

/* For malloc, free */
# include <stdlib.h>

/* For write, access, read, close, fork, dup, dup2, pipe */
# include <unistd.h>

/* For open */
# include <fcntl.h>

/* For wait, waitpid, wait3, wait4 */
# include <sys/wait.h>

/* For signal, sigaction, sigemptyset, sigaddset, kill */
# include <signal.h>

/* For getcwd, chdir */
# include <unistd.h>

/* For stat, lstat, fstat */
# include <sys/stat.h>

/* For unlink */
# include <unistd.h>

/* For execve */
# include <unistd.h>

/* For opendir, readdir, closedir */
# include <dirent.h>

/* For strerror, perror */
# include <errno.h>
# include <string.h>

/* For isatty, ttyname, ttyslot */
# include <unistd.h>

/* For ioctl */
# include <sys/ioctl.h>

/* For getenv */
# include <stdlib.h>

/* For tcsetattr, tcgetattr */
# include <termios.h>

/* For tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs */
# include <curses.h>
# include <term.h>

#endif
#ifndef HEADER_H
# define HEADER_H

#define PROMPT "minishell> "
#define EXIT_CMD "exit"
#define PWD_CMD "pwd"
#define ENV_CMD "env"
#define UNSET_CMD "unset"
#define EXPORT_CMD "export"
#define ECHO_CMD "echo"

#define MAX_ENV_VARS 1024

extern char **g_envp;  // Declare the global variable

typedef struct s_command {
  char *name;
  void (*func)(void);
} t_command;

// Init
void init_envp(char **envp);

// Commands
void execute_command(char *input, char **envp);
void cmd_export(char **args);
void cmd_pwd(void);
void cmd_exit(void);
void cmd_env(char **envp);

// Utils
char *expand_dollar(char *input, char **envp);

/* For libft */
# include "../libs/libft/libft.h"

/* For printf */
# include <stdio.h>

/* For readline, rl_clear_history, rl_on_new_line, rl_replace_line,
	rl_redisplay, add_history */
# include <readline/history.h>
# include <readline/readline.h>

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

/* For exit */
# include <stdlib.h>

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
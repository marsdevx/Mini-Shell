#ifndef EXEC_H
# define EXEC_H

#include "../init/init.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/* Execution context structure */
typedef struct s_exec_ctx {
    int     stdin_backup;
    int     stdout_backup;
    int     last_exit_status;
    char    **envp;
    t_info  *info;
} t_exec_ctx;

/* Built-in function prototype */
typedef int (*t_builtin_func)(char **args, t_exec_ctx *ctx);

/* Built-in structure */
typedef struct s_builtin {
    char            *name;
    t_builtin_func  func;
} t_builtin;

/* Main execution functions */
int     execute_commands(t_list *groups, char **envp, t_info *info);
int     execute_single_command(t_group *grp, t_exec_ctx *ctx);
int     execute_external(char **args, t_exec_ctx *ctx);

/* PATH resolution */
char    *resolve_command_path(const char *cmd);
int     is_path_command(const char *cmd);

/* Redirection handling */
int     setup_redirections(char ***args);
void    restore_standard_fds(t_exec_ctx *ctx);
int     handle_input_redirect(const char *filename);
int     handle_output_redirect(const char *filename);
int     handle_append_redirect(const char *filename);
int     handle_heredoc(const char *delimiter);

/* Built-in commands */
int     is_builtin(const char *cmd);
int     execute_builtin(char **args, t_exec_ctx *ctx);
int     builtin_cd(char **args, t_exec_ctx *ctx);
int     builtin_echo(char **args, t_exec_ctx *ctx);
int     builtin_pwd(char **args, t_exec_ctx *ctx);
int     builtin_export(char **args, t_exec_ctx *ctx);
int     builtin_unset(char **args, t_exec_ctx *ctx);
int     builtin_env(char **args, t_exec_ctx *ctx);
int     builtin_exit(char **args, t_exec_ctx *ctx);

/* Helper functions */
char    **group_to_argv(t_group *grp);
void    free_argv(char **argv);
int     count_args(t_list *args);

#endif
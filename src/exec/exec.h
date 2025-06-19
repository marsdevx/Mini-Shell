#ifndef EXEC_H
# define EXEC_H

#include "../init/init.h"

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

/* Helper functions */
char    **group_to_argv(t_group *grp);
void    free_argv(char **argv);
int     count_args(t_list *args);

#endif
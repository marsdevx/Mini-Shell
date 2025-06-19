#ifndef EXEC_H
# define EXEC_H

#include "../init/init.h"

int     execute_commands(t_list *groups, char **envp, t_info *info);
int     execute_single_command(t_group *grp, t_exec_ctx *ctx);
int     execute_external(char **args, t_exec_ctx *ctx);
char    *resolve_command_path(const char *cmd);
int     setup_redirections(char ***args);
int     handle_input_redirect(const char *filename);
int     handle_output_redirect(const char *filename);
int     handle_append_redirect(const char *filename);
int     handle_heredoc(const char *delimiter);
int     count_args(t_list *args);

#endif
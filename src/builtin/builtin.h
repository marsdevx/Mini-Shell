#ifndef BUILTIN_H
# define BUILTIN_H

#  include "../init/init.h"

int     is_builtin(const char *cmd);
int     execute_builtin(char **args, t_exec_ctx *ctx);
int     builtin_cd(char **args, t_exec_ctx *ctx);
int     builtin_echo(char **args, t_exec_ctx *ctx);
int     builtin_pwd(char **args, t_exec_ctx *ctx);
int     builtin_export(char **args, t_exec_ctx *ctx);
int     builtin_unset(char **args, t_exec_ctx *ctx);
int     builtin_env(char **args, t_exec_ctx *ctx);
int     builtin_exit(char **args, t_exec_ctx *ctx);

#endif
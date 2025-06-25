/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 23:44:24 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
# define HEADER_H

// Global Libs
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
#include <stddef.h>

// Local Libs
# include "../../libs/libft/libft.h"

// Structs
typedef enum e_type
{
	WORD,
	FIELD,
	EXP_FIELD,
	SEP,
	PIPE,
	REDIRECT_IN,
	REDIRECT_OUT,
	REDIRECT_APPEND,
	HEREDOC
}					t_e_type;

typedef struct s_token
{
	t_e_type		type;
	char			*value;
	int				value_len;
}					t_token;

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;

typedef struct s_info
{
	int				exit_f;
	int				last_exit_status;
}					t_info;

typedef struct s_command
{
	char			*arg;
}					t_command;

typedef struct s_group
{
	t_list			*argv;
}					t_group;

typedef struct s_exec_ctx
{
	int				stdin_backup;
	int				stdout_backup;
	int				last_exit_status;
	char			**envp;
	t_info			*info;
}					t_exec_ctx;

typedef int			(*t_builtin_func)(char **args, t_exec_ctx *ctx);

typedef struct s_builtin
{
	char			*name;
	t_builtin_func	func;
}					t_builtin;

// builtin.h
int	is_builtin(const char *cmd);
int	execute_builtin(char **args, t_exec_ctx *ctx);
int	builtin_cd(char **args, t_exec_ctx *ctx);
int	builtin_echo(char **args, t_exec_ctx *ctx);
int	builtin_pwd(char **args, t_exec_ctx *ctx);
int	builtin_export(char **args, t_exec_ctx *ctx);
int	builtin_unset(char **args, t_exec_ctx *ctx);
int	builtin_env(char **args, t_exec_ctx *ctx);
int	builtin_exit(char **args, t_exec_ctx *ctx);

// exec.h
int		execute_commands(t_list *groups, char **envp, t_info *info);
int		execute_single_command(t_group *grp, t_exec_ctx *ctx);
int		execute_external(char **args, t_exec_ctx *ctx);
int		execute_pipeline(t_list *groups, t_exec_ctx *ctx);
char	**group_to_argv(t_group *grp);
void	free_argv(char **argv);
char	*resolve_command_path(const char *cmd);
int		setup_redirections(char ***args);
int		handle_input_redirect(const char *filename);
int		handle_output_redirect(const char *filename);
int		handle_append_redirect(const char *filename);
int		handle_heredoc(const char *delimiter);
int		count_args(t_list *args);

// lexer.h
int		quotes_check(char *input);
t_list	*lexer(char *input);

// parser.h
t_list	*parser(t_list *tokens);
void	free_groups(t_list **groups);

// reader.h
int		ft_init(t_info *info);
void	handle_sigint(int sig);
char	*ft_readline(const char *prompt);

// utils.h
t_list	*ft_lstnew(void *content);
void	ft_lstadd_back(t_list **lst, t_list *new);
void	ft_free_tokens(t_list **tokens);
char	*ft_strndup(const char *s, size_t n);
int ft_setenv(const char *name, const char *value, int overwrite);
char *ft_strcpy(char *dest, const char *src);
char *ft_strncpy(char *dest, const char *src, size_t n);
int ft_strcmp(const char *s1, const char *s2);
char *ft_strcat(char *dest, const char *src);
char *ft_strtok(char *str, const char *delim);
long ft_strtol(const char *str, char **endptr, int base);

#endif
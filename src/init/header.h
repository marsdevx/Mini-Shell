/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 19:07:07 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
# define HEADER_H

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
# include <limits.h>

# include "../../libs/libft/libft.h"

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
	char			**env;
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

int	is_valid_identifier(const char *str);
int is_valid_number(char *str);
int					is_builtin(const char *cmd);
int					execute_builtin(char **args, t_exec_ctx *ctx);
int					builtin_cd(char **args, t_exec_ctx *ctx);
int					builtin_echo(char **args, t_exec_ctx *ctx);
int					builtin_pwd(char **args, t_exec_ctx *ctx);
int					builtin_export(char **args, t_exec_ctx *ctx);
int					builtin_unset(char **args, t_exec_ctx *ctx);
int					builtin_env(char **args, t_exec_ctx *ctx);
int					builtin_exit(char **args, t_exec_ctx *ctx);

int	process_token(char **ptr, t_token *token);
int					execute_commands(t_list *groups, t_info *info);
int					execute_single_command(t_group *grp, t_exec_ctx *ctx);
int					execute_external(char **args, t_exec_ctx *ctx);
int					execute_pipeline(t_list *groups, t_exec_ctx *ctx);
char				**group_to_argv(t_group *grp);
void				free_argv(char **argv);
char				*resolve_command_path(const char *cmd, char **env);
int					setup_redirections(char ***args);
int					handle_input_redirect(const char *filename);
int					handle_output_redirect(const char *filename);
int					handle_append_redirect(const char *filename);
int					handle_heredoc(const char *delimiter);
int					count_args(t_list *args);

int					quotes_check(char *input);
t_list				*lexer(char *input);

t_list				*parser(t_list *tokens, char **env);
t_list				*tokens_to_groups(t_list *tok_lst, char **env);

t_command			*new_command(const char *arg);
t_group				*new_group(void);
int					add_argument(t_group *grp, const char *arg);
void				free_groups(t_list **groups);
char				*join_strings(char *s1, char *s2);

char				*expand_env_var(const char *var_name, char **env);
char				*get_var_name(const char *str, int *len);
char				*expand_word_env_process_var(const char **p, char **result,
						char **env);
char				*expand_word_env_process_char(const char **p, char **result,
						char c);

char				*expand_word_env(const char *src, char **env);
char				*concatenate_text_tokens(t_list **scan, char **env);

int					handle_redirect_create_group(t_group **cur,
						t_list **groups);
int					handle_redirect(t_token *tk, t_list *tok_lst,
						t_group **cur, t_list **groups);
int					handle_text_create_group(t_group **cur, t_list **groups);
int					handle_text(t_list **tok_lst, t_group **cur,
						t_list **groups, char **env);

int					is_redirect(t_e_type t);
int					is_text(t_e_type t);
int					is_valid_var_char(char c, int first);

int					ft_init(t_info *info);
void				handle_sigint(int sig);
char				*ft_readline(const char *prompt);

t_list				*ft_lstnew(void *content);
void				ft_lstadd_back(t_list **lst, t_list *new);
void				ft_free_tokens(t_list **tokens);
char				*ft_strndup(const char *s, size_t n);
int					ft_strcmp(const char *s1, const char *s2);
void				write_error(const char *msg);
void				write_error_with_arg(const char *prefix, const char *arg,
						const char *suffix);
char				*ft_strncpy(char *dest, const char *src, size_t n);
char				*ft_strcpy(char *dst, const char *src);
char				*ft_strcat(char *s1, const char *s2);
char				**init_env_copy(char **envp);
void				free_env_copy(char **env);
char				**set_env_var(char **env, const char *name,
						const char *value, int overwrite);
char				**unset_env_var(char **env, const char *name);
char				*get_env_value(char **env, const char *name);

#endif
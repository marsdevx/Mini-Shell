/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/24 19:27:40 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INIT_H
# define INIT_H

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
}					e_type;

typedef struct s_token
{
	e_type			type;
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

#endif
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

#ifndef EXEC_H
# define EXEC_H

# include "../init/init.h"

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

#endif
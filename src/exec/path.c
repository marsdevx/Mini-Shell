/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 23:42:40 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

int	is_path_command(const char *cmd)
{
	return (strchr(cmd, '/') != NULL);
}

static char	*try_path(const char *dir, const char *cmd)
{
	size_t	dir_len;
	size_t	cmd_len;
	char	*full_path;

	dir_len = ft_strlen(dir);
	cmd_len = ft_strlen(cmd);
	full_path = malloc(dir_len + cmd_len + 2);
	if (!full_path)
		return (NULL);
	ft_strcpy(full_path, dir);
	if (dir_len > 0 && dir[dir_len - 1] != '/')
		ft_strcat(full_path, "/");
	ft_strcat(full_path, cmd);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}

char	*resolve_command_path(const char *cmd)
{
	char	*path_env;
	char	*path_copy;
	char	*dir;
	char	*result;

	if (is_path_command(cmd))
	{
		return (ft_strdup(cmd));
	}
	path_env = getenv("PATH");
	if (!path_env)
	{
		return (NULL);
	}
	path_copy = ft_strdup(path_env);
	if (!path_copy)
		return (NULL);
	dir = ft_strtok(path_copy, ":");
	result = NULL;
	while (dir && !result)
	{
		result = try_path(dir, cmd);
		dir = ft_strtok(NULL, ":");
	}
	free(path_copy);
	return (result);
}

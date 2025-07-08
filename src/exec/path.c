/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 22:33:54 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

int	is_path_command(const char *cmd)
{
	return (ft_strchr(cmd, '/') != NULL);
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

static char	*get_next_path_dir(char **path_ptr)
{
	char	*start;
	char	*end;
	char	*dir;
	size_t	len;

	if (!path_ptr || !*path_ptr || **path_ptr == '\0')
		return (NULL);
	start = *path_ptr;
	end = ft_strchr(start, ':');
	if (end)
	{
		len = end - start;
		*path_ptr = end + 1;
	}
	else
	{
		len = ft_strlen(start);
		*path_ptr = start + len;
	}
	dir = malloc(len + 1);
	if (!dir)
		return (NULL);
	ft_strncpy(dir, start, len);
	dir[len] = '\0';
	return (dir);
}

char	*resolve_command_path(const char *cmd, char **env)
{
	char	*path_env;
	char	*path_copy;
	char	*path_ptr;
	char	*dir;
	char	*result;

	if (is_path_command(cmd))
		return (ft_strdup(cmd));
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	path_copy = ft_strdup(path_env);
	if (!path_copy)
		return (NULL);
	path_ptr = path_copy;
	result = NULL;
	while (!result && (dir = get_next_path_dir(&path_ptr)) != NULL)
	{
		result = try_path(dir, cmd);
		free(dir);
	}
	free(path_copy);
	return (result);
}

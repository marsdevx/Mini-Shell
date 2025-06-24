/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/24 20:01:09 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"


int is_path_command(const char *cmd)
{
    return (strchr(cmd, '/') != NULL);
}


static char *try_path(const char *dir, const char *cmd)
{
    size_t dir_len = strlen(dir);
    size_t cmd_len = strlen(cmd);
    char *full_path = malloc(dir_len + cmd_len + 2);
    
    if (!full_path)
        return NULL;
    

    strcpy(full_path, dir);
    if (dir_len > 0 && dir[dir_len - 1] != '/')
        strcat(full_path, "/");
    strcat(full_path, cmd);
    

    if (access(full_path, X_OK) == 0)
        return full_path;
    
    free(full_path);
    return NULL;
}


char *resolve_command_path(const char *cmd)
{

    if (is_path_command(cmd))
    {

        return strdup(cmd);
    }
    

    char *path_env = getenv("PATH");
    if (!path_env)
    {

        return NULL;
    }
    

    char *path_copy = strdup(path_env);
    if (!path_copy)
        return NULL;
    

    char *dir = strtok(path_copy, ":");
    char *result = NULL;
    
    while (dir && !result)
    {
        result = try_path(dir, cmd);
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return result;
}
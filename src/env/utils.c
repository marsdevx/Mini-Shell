/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 22:19:42 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

// Free the environment copy
void free_env_copy(char **env)
{
    int i;
    
    if (!env)
        return;
    
    i = 0;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}

// Find environment variable
int find_env_var(char **env, const char *name)
{
    int i = 0;
    size_t name_len = ft_strlen(name);
    
    if (!env)
        return (-1);
    
    while (env[i])
    {
        if (ft_strncmp(env[i], name, name_len) == 0 &&
            env[i][name_len] == '=')
            return (i);
        i++;
    }
    return (-1);
}

// Unset environment variable
char **unset_env_var(char **env, const char *name)
{
    int index;
    int i;
    
    if (!name || !env)
        return (env);
    
    index = find_env_var(env, name);
    if (index < 0)
        return (env);
    
    // Free the variable
    free(env[index]);
    
    // Shift remaining variables
    i = index;
    while (env[i + 1])
    {
        env[i] = env[i + 1];
        i++;
    }
    env[i] = NULL;
    
    return (env);
}

// Get environment variable value
char *get_env_value(char **env, const char *name)
{
    int index;
    char *value;
    
    if (!name || !env)
        return (NULL);
    
    index = find_env_var(env, name);
    if (index < 0)
        return (NULL);
    
    // Find the '=' and return pointer to value after it
    value = ft_strchr(env[index], '=');
    if (value)
        return (value + 1);
    
    return (NULL);
}
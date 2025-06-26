/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/26 20:42:28 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

// Initialize custom environment from system environment
char **init_env_copy(char **envp)
{
    int count = 0;
    int i;
    char **env_copy;
    
    // Count environment variables
    while (envp[count])
        count++;
    
    // Allocate memory for environment copy
    env_copy = malloc(sizeof(char *) * (count + 1));
    if (!env_copy)
        return (NULL);
    
    // Copy environment variables
    for (i = 0; i < count; i++)
    {
        env_copy[i] = ft_strdup(envp[i]);
        if (!env_copy[i])
        {
            // Clean up on error
            while (--i >= 0)
                free(env_copy[i]);
            free(env_copy);
            return (NULL);
        }
    }
    env_copy[count] = NULL;
    
    return (env_copy);
}

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
static int find_env_var(char **env, const char *name)
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

// Count environment variables
static int count_env_vars(char **env)
{
    int count = 0;
    
    if (!env)
        return (0);
    
    while (env[count])
        count++;
    return (count);
}

// Set environment variable
char **set_env_var(char **env, const char *name, const char *value, int overwrite)
{
    char *new_var;
    char **new_env;
    int index;
    int count;
    int i;
    
    if (!name || !value || ft_strchr(name, '='))
        return (env);
    
    // Create new variable string
    new_var = malloc(ft_strlen(name) + ft_strlen(value) + 2);
    if (!new_var)
        return (env);
    ft_strcpy(new_var, name);
    ft_strcat(new_var, "=");
    ft_strcat(new_var, value);
    
    // Check if variable already exists
    index = find_env_var(env, name);
    if (index >= 0)
    {
        if (!overwrite)
        {
            free(new_var);
            return (env);
        }
        // Replace existing variable
        free(env[index]);
        env[index] = new_var;
        return (env);
    }
    
    // Add new variable
    count = count_env_vars(env);
    new_env = malloc(sizeof(char *) * (count + 2));
    if (!new_env)
    {
        free(new_var);
        return (env);
    }
    
    // Copy existing variables
    for (i = 0; i < count; i++)
        new_env[i] = env[i];
    
    // Add new variable
    new_env[count] = new_var;
    new_env[count + 1] = NULL;
    
    // Free old array
    free(env);
    
    return (new_env);
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
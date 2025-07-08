/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 22:32:34 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

char	**init_env_copy(char **envp)
{
	int		count;
	int		i;
	char	**env_copy;

	count = 0;
	while (envp[count])
		count++;
	env_copy = malloc(sizeof(char *) * (count + 1));
	if (!env_copy)
		return (NULL);
	for (i = 0; i < count; i++)
	{
		env_copy[i] = ft_strdup(envp[i]);
		if (!env_copy[i])
		{
			while (--i >= 0)
				free(env_copy[i]);
			free(env_copy);
			return (NULL);
		}
	}
	env_copy[count] = NULL;
	return (env_copy);
}

static int	count_env_vars(char **env)
{
	int	count;

	count = 0;
	if (!env)
		return (0);
	while (env[count])
		count++;
	return (count);
}

char	**set_env_var(char **env, const char *name, const char *value,
		int overwrite)
{
	char	*new_var;
	char	**new_env;
	int		index;
	int		count;
	int		i;

	if (!name || !value || ft_strchr(name, '='))
		return (env);
	new_var = malloc(ft_strlen(name) + ft_strlen(value) + 2);
	if (!new_var)
		return (env);
	ft_strcpy(new_var, name);
	ft_strcat(new_var, "=");
	ft_strcat(new_var, value);
	index = find_env_var(env, name);
	if (index >= 0)
	{
		if (!overwrite)
		{
			free(new_var);
			return (env);
		}
		free(env[index]);
		env[index] = new_var;
		return (env);
	}
	count = count_env_vars(env);
	new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
	{
		free(new_var);
		return (env);
	}
	for (i = 0; i < count; i++)
		new_env[i] = env[i];
	new_env[count] = new_var;
	new_env[count + 1] = NULL;
	free(env);
	return (new_env);
}

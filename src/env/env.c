/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 22:23:22 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

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

static char	*build_env_entry(const char *name, const char *value)
{
	char	*entry;

	entry = malloc(ft_strlen(name) + ft_strlen(value) + 2);
	if (!entry)
		return (NULL);
	ft_strcpy(entry, name);
	ft_strcat(entry, "=");
	ft_strcat(entry, value);
	return (entry);
}

static char	**append_env_entry(char **env, char *new_entry)
{
	int		count;
	char	**new_env;
	int		i;

	count = count_env_vars(env);
	new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_env[i] = env[i];
		++i;
	}
	new_env[count] = new_entry;
	new_env[count + 1] = NULL;
	free(env);
	return (new_env);
}

static char	**update_env(char **env, char *new_entry, int index, int overwrite)
{
	if (index >= 0)
	{
		if (!overwrite)
		{
			free(new_entry);
			return (env);
		}
		free(env[index]);
		env[index] = new_entry;
		return (env);
	}
	return (append_env_entry(env, new_entry));
}

char	**set_env_var(char **env, const char *name, const char *value,
		int overwrite)
{
	char	*new_entry;
	char	**result;
	int		index;

	if (!name || !value || ft_strchr(name, '='))
		return (env);
	new_entry = build_env_entry(name, value);
	if (!new_entry)
		return (env);
	index = find_env_var(env, name);
	result = update_env(env, new_entry, index, overwrite);
	if (!result)
	{
		free(new_entry);
		return (env);
	}
	return (result);
}

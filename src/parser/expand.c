/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 18:43:48 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

char	*expand_env_var(const char *var_name, char **env)
{
	const char	*val;

	if (ft_strncmp(var_name, "?", 2) == 0)
	{
		val = get_env_value(env, "?");
		if (!val)
			val = "0";
	}
	else
	{
		val = get_env_value(env, var_name);
		if (!val)
			val = "";
	}
	return (ft_strdup(val));
}

char	*get_var_name(const char *str, int *len)
{
	char	*var_name;
	int		i;

	if (*str == '?')
	{
		*len = 1;
		return (ft_strdup("?"));
	}
	i = 0;
	while (str[i] && is_valid_var_char(str[i], i == 0))
		i++;
	*len = i;
	if (i == 0)
		return (NULL);
	var_name = malloc(i + 1);
	if (!var_name)
		return (NULL);
	ft_strlcpy(var_name, str, i + 1);
	return (var_name);
}

char	*expand_word_env_process_var(const char **p, char **result, char **env)
{
	char	*var_name;
	char	*var_value;
	char	*new_result;
	int		var_len;

	var_name = get_var_name(*p, &var_len);
	if (!var_name)
	{
		free(*result);
		return (NULL);
	}
	var_value = expand_env_var(var_name, env);
	free(var_name);
	if (!var_value)
	{
		free(*result);
		return (NULL);
	}
	new_result = join_strings(*result, var_value);
	free(*result);
	free(var_value);
	if (!new_result)
		return (NULL);
	*p += var_len;
	return (new_result);
}

char	*expand_word_env_process_char(const char **p, char **result, char c)
{
	char	*temp;
	char	*new_result;

	temp = malloc(2);
	if (!temp)
	{
		free(*result);
		return (NULL);
	}
	temp[0] = c;
	temp[1] = '\0';
	new_result = join_strings(*result, temp);
	free(*result);
	free(temp);
	if (!new_result)
		return (NULL);
	(*p)++;
	return (new_result);
}

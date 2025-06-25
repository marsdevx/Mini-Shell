#include "../init/header.h"

// Global environment copy
static char **g_env = NULL;
static int g_env_size = 0;
static int g_env_capacity = 0;

int	init_env(char **envp)
{
	int	count = 0;
	int	i;

	while (envp[count])
		count++;
	g_env_capacity = count + 50;
	g_env = malloc(sizeof(char *) * (g_env_capacity + 1));
	if (!g_env)
		return (-1);
	for (i = 0; i < count; i++)
	{
		g_env[i] = malloc(strlen(envp[i]) + 1);
		if (!g_env[i])
		{
			while (--i >= 0)
				free(g_env[i]);
			free(g_env);
			return (-1);
		}
		strcpy(g_env[i], envp[i]);
	}
	g_env[count] = NULL;
	g_env_size = count;
	return (0);
}

void	cleanup_env(void)
{
	int	i;

	if (!g_env)
		return;
	for (i = 0; i < g_env_size; i++)
		free(g_env[i]);
	free(g_env);
	g_env = NULL;
	g_env_size = 0;
	g_env_capacity = 0;
}

char	**get_env(void)
{
	return (g_env);
}

int	ft_setenv(const char *name, const char *value, int overwrite)
{
	char	*new_entry;
	int		i;
	int		name_len;

	if (!name || !value || strchr(name, '='))
		return (-1);
	name_len = strlen(name);
	for (i = 0; i < g_env_size; i++)
	{
		if (strncmp(g_env[i], name, name_len) == 0 && g_env[i][name_len] == '=')
		{
			if (!overwrite)
				return (0);
			new_entry = malloc(name_len + strlen(value) + 2);
			if (!new_entry)
				return (-1);
			strcpy(new_entry, name);
			strcat(new_entry, "=");
			strcat(new_entry, value);
			free(g_env[i]);
			g_env[i] = new_entry;
			return (0);
		}
	}
	if (g_env_size >= g_env_capacity)
	{
		char **new_env = malloc(sizeof(char *) * (g_env_capacity * 2 + 1));
		if (!new_env)
			return (-1);
		for (i = 0; i < g_env_size; i++)
			new_env[i] = g_env[i];
		free(g_env);
		g_env = new_env;
		g_env_capacity *= 2;
	}
	new_entry = malloc(name_len + strlen(value) + 2);
	if (!new_entry)
		return (-1);
	strcpy(new_entry, name);
	strcat(new_entry, "=");
	strcat(new_entry, value);
	g_env[g_env_size] = new_entry;
	g_env_size++;
	g_env[g_env_size] = NULL;
	return (0);
}

int	ft_unsetenv(const char *name)
{
	int	i, j;
	int	name_len;

	if (!name || strchr(name, '='))
		return (-1);
	name_len = strlen(name);
	for (i = 0; i < g_env_size; i++)
	{
		if (strncmp(g_env[i], name, name_len) == 0 && g_env[i][name_len] == '=')
		{
			free(g_env[i]);
			for (j = i; j < g_env_size - 1; j++)
				g_env[j] = g_env[j + 1];
			g_env_size--;
			g_env[g_env_size] = NULL;
			return (0);
		}
	}
	return (0);
}

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;
	size_t	copy_size;

	if (!ptr)
		return (malloc(new_size));
	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	copy_size = (old_size < new_size) ? old_size : new_size;
	memcpy(new_ptr, ptr, copy_size);
	free(ptr);
	return (new_ptr);
}
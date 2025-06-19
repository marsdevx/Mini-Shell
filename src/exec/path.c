#include "../init/header.h"

/* Check if command contains path separator */
int is_path_command(const char *cmd)
{
    return (strchr(cmd, '/') != NULL);
}

/* Try to find executable in a specific directory */
static char *try_path(const char *dir, const char *cmd)
{
    size_t dir_len = strlen(dir);
    size_t cmd_len = strlen(cmd);
    char *full_path = malloc(dir_len + cmd_len + 2);
    
    if (!full_path)
        return NULL;
    
    /* Build full path */
    strcpy(full_path, dir);
    if (dir_len > 0 && dir[dir_len - 1] != '/')
        strcat(full_path, "/");
    strcat(full_path, cmd);
    
    /* Check if file exists and is executable */
    if (access(full_path, X_OK) == 0)
        return full_path;
    
    free(full_path);
    return NULL;
}

/* Resolve command to full path */
char *resolve_command_path(const char *cmd)
{
    /* If command contains '/', treat as path */
    if (is_path_command(cmd))
    {
        if (access(cmd, X_OK) == 0)
            return strdup(cmd);
        return NULL;
    }
    
    /* Get PATH environment variable */
    char *path_env = getenv("PATH");
    if (!path_env)
        path_env = "/usr/local/bin:/usr/bin:/bin";
    
    /* Make a copy of PATH to tokenize */
    char *path_copy = strdup(path_env);
    if (!path_copy)
        return NULL;
    
    /* Try each directory in PATH */
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
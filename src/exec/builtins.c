#include "../init/header.h"

/* Built-in command table */
static const t_builtin g_builtins[] = {
    {"cd", builtin_cd},
    {"echo", builtin_echo},
    {"pwd", builtin_pwd},
    {"export", builtin_export},
    {"unset", builtin_unset},
    {"env", builtin_env},
    {"exit", builtin_exit},
    {NULL, NULL}
};

/* Check if command is a built-in */
int is_builtin(const char *cmd)
{
    for (int i = 0; g_builtins[i].name; i++)
    {
        if (strcmp(cmd, g_builtins[i].name) == 0)
            return 1;
    }
    return 0;
}

/* Execute built-in command */
int execute_builtin(char **args, t_exec_ctx *ctx)
{
    for (int i = 0; g_builtins[i].name; i++)
    {
        if (strcmp(args[0], g_builtins[i].name) == 0)
            return g_builtins[i].func(args, ctx);
    }
    return 127; /* Command not found */
}

/* Built-in: cd */
int builtin_cd(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    char *path;
    
    if (!args[1])
    {
        /* No argument: go to HOME */
        path = getenv("HOME");
        if (!path)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }
    else if (strcmp(args[1], "-") == 0)
    {
        /* cd - : go to previous directory */
        path = getenv("OLDPWD");
        if (!path)
        {
            fprintf(stderr, "cd: OLDPWD not set\n");
            return 1;
        }
        printf("%s\n", path);
    }
    else
    {
        path = args[1];
    }
    
    /* Save current directory */
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        setenv("OLDPWD", cwd, 1);
    
    /* Change directory */
    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }
    
    /* Update PWD */
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        setenv("PWD", cwd, 1);
    
    return 0;
}

/* Built-in: echo */
int builtin_echo(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    int i = 1;
    int newline = 1;
    
    /* Check for -n flag */
    if (args[1] && strcmp(args[1], "-n") == 0)
    {
        newline = 0;
        i = 2;
    }
    
    /* Print arguments */
    while (args[i])
    {
        printf("%s", args[i]);
        if (args[i + 1])
            printf(" ");
        i++;
    }
    
    if (newline)
        printf("\n");
    
    return 0;
}

/* Built-in: pwd */
int builtin_pwd(char **args, t_exec_ctx *ctx)
{
    (void)args;
    (void)ctx;
    char cwd[1024];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
        return 0;
    }
    else
    {
        perror("pwd");
        return 1;
    }
}

/* Built-in: env */
int builtin_env(char **args, t_exec_ctx *ctx)
{
    (void)args;
    
    if (!ctx->envp)
        return 0;
    
    for (int i = 0; ctx->envp[i]; i++)
        printf("%s\n", ctx->envp[i]);
    
    return 0;
}

/* Built-in: export */
int builtin_export(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    
    /* No arguments: print all exported variables */
    if (!args[1])
    {
        extern char **environ;
        for (int i = 0; environ[i]; i++)
            printf("declare -x %s\n", environ[i]);
        return 0;
    }
    
    /* Export each argument */
    for (int i = 1; args[i]; i++)
    {
        char *equals = strchr(args[i], '=');
        if (equals)
        {
            /* VAR=value format */
            *equals = '\0';
            setenv(args[i], equals + 1, 1);
            *equals = '=';
        }
        else
        {
            /* Just mark as exported (already in env) */
            char *value = getenv(args[i]);
            if (value)
                setenv(args[i], value, 1);
        }
    }
    
    return 0;
}

/* Built-in: unset */
int builtin_unset(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    
    for (int i = 1; args[i]; i++)
        unsetenv(args[i]);
    
    return 0;
}

/* Built-in: exit */
int builtin_exit(char **args, t_exec_ctx *ctx)
{
    int exit_code = ctx->last_exit_status;
    
    /* Parse exit code if provided */
    if (args[1])
    {
        char *endptr;
        long val = strtol(args[1], &endptr, 10);
        
        if (*endptr != '\0' || val < 0 || val > 255)
        {
            fprintf(stderr, "exit: %s: numeric argument required\n", args[1]);
            exit_code = 2;
        }
        else
        {
            exit_code = (int)val;
        }
    }
    
    /* Set exit flag to 0 to signal main loop to exit */
    ctx->info->exit_f = 0;
    
    printf("exit\n");
    /* Don't call exit() here - let main loop handle it */
    return exit_code;
}
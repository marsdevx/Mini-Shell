#include "../init/header.h"

/* Static builtin table - encapsulated in getter function */
static const t_builtin *get_builtin_table(void)
{
    static const t_builtin builtins[] = {
        {"cd", builtin_cd},
        {"echo", builtin_echo},
        {"pwd", builtin_pwd},
        {"export", builtin_export},
        {"unset", builtin_unset},
        {"env", builtin_env},
        {"exit", builtin_exit},
        {NULL, NULL}
    };
    return builtins;
}

/* Check if command is a built-in */
int is_builtin(const char *cmd)
{
    const t_builtin *builtins = get_builtin_table();
    
    for (int i = 0; builtins[i].name; i++)
    {
        if (strcmp(cmd, builtins[i].name) == 0)
            return 1;
    }
    return 0;
}

/* Execute built-in command */
int execute_builtin(char **args, t_exec_ctx *ctx)
{
    const t_builtin *builtins = get_builtin_table();
    
    for (int i = 0; builtins[i].name; i++)
    {
        if (strcmp(args[0], builtins[i].name) == 0)
        {
            int status = builtins[i].func(args, ctx);
            ctx->last_exit_status = status;
            return status;
        }
    }
    return 127; /* Command not found */
}

/* Helper function to check if string is valid identifier */
static int is_valid_identifier(const char *str)
{
    if (!str || !*str)
        return 0;
    
    /* First character must be letter or underscore */
    if (!isalpha((unsigned char)*str) && *str != '_')
        return 0;
    
    /* Rest must be alphanumeric or underscore */
    for (const char *p = str + 1; *p; p++)
    {
        if (!isalnum((unsigned char)*p) && *p != '_')
            return 0;
    }
    
    return 1;
}

/* Built-in: cd */
int builtin_cd(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    char *path;
    
    /* Check for too many arguments */
    if (args[1] && args[2])
    {
        fprintf(stderr, "bash: cd: too many arguments\n");
        return 1;
    }
    
    if (!args[1])
    {
        /* No argument: go to HOME */
        path = getenv("HOME");
        if (!path)
        {
            fprintf(stderr, "bash: cd: HOME not set\n");
            return 1;
        }
    }
    else if (strcmp(args[1], "-") == 0)
    {
        /* cd - : go to previous directory */
        path = getenv("OLDPWD");
        if (!path)
        {
            fprintf(stderr, "bash: cd: OLDPWD not set\n");
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
        fprintf(stderr, "bash: cd: %s: No such file or directory\n", path);
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
    
    /* Check for all -n flags */
    while (args[i] && strcmp(args[i], "-n") == 0) {
        newline = 0;
        i++;
    }
    
    /* Print arguments */
    while (args[i]) {
        printf("%s", args[i]);
        if (args[i + 1])
            printf(" ");
        i++;
    }
    
    if (newline)
        printf("\n");
    
    fflush(stdout);
    return 0;
}

/* Built-in: pwd */
int builtin_pwd(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    char cwd[1024];
    
    /* pwd doesn't take arguments in bash */
    if (args[1])
    {
        /* bash just ignores extra arguments for pwd */
        /* No error is printed */
    }
    
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
    (void)ctx;
    
    extern char **environ;
    
    for (int i = 0; environ[i]; i++)
        printf("%s\n", environ[i]);
    
    return 0;
}

/* Built-in: export */
int builtin_export(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    int ret = 0;
    
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
            
            if (!is_valid_identifier(args[i]))
            {
                fprintf(stderr, "bash: export: `%s': not a valid identifier\n", args[i]);
                *equals = '=';
                ret = 1;
                continue;
            }
            
            /* Restore the equals sign */
            *equals = '=';
            
            /* Extract variable name and value */
            char *var_name = malloc(equals - args[i] + 1);
            if (!var_name)
                return 1;
            strncpy(var_name, args[i], equals - args[i]);
            var_name[equals - args[i]] = '\0';
            
            setenv(var_name, equals + 1, 1);
            free(var_name);
        }
        else
        {
            /* Just the variable name */
            if (!is_valid_identifier(args[i]))
            {
                fprintf(stderr, "bash: export: `%s': not a valid identifier\n", args[i]);
                ret = 1;
                continue;
            }
            
            /* Mark as exported (if it exists) */
            char *value = getenv(args[i]);
            if (value)
                setenv(args[i], value, 1);
        }
    }
    
    return ret;
}

/* Built-in: unset */
int builtin_unset(char **args, t_exec_ctx *ctx)
{
    (void)ctx;
    
    /* No arguments is valid, just return 0 */
    if (!args[1])
        return 0;
    
    for (int i = 1; args[i]; i++)
        unsetenv(args[i]);
    
    return 0;
}

/* Built-in: exit */
int builtin_exit(char **args, t_exec_ctx *ctx)
{
    int exit_code = ctx->last_exit_status;
    
    printf("exit\n");
    
    /* Check for too many arguments first */
    if (args[1] && args[2])
    {
        fprintf(stderr, "bash: exit: too many arguments\n");
        return 1;  /* Don't exit the shell, just return error */
    }
    
    /* Parse exit code if provided */
    if (args[1])
    {
        char *endptr;
        long val = strtol(args[1], &endptr, 10);
        
        /* Check if it's a valid number */
        if (*endptr != '\0')
        {
            fprintf(stderr, "bash: exit: %s: numeric argument required\n", args[1]);
            exit_code = 2;
        }
        else
        {
            /* Handle the modulo 256 behavior */
            exit_code = ((val % 256) + 256) % 256;
        }
    }
    
    /* Set exit flag to 0 to signal main loop to exit */
    ctx->info->exit_f = 0;
    ctx->info->last_exit_status = exit_code;
    
    return exit_code;
}
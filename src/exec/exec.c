#include "../init/header.h"

/* Convert linked list of arguments to argv array */
char **group_to_argv(t_group *grp)
{
    int count = count_args(grp->argv);
    char **argv = malloc(sizeof(char *) * (count + 1));
    if (!argv)
        return NULL;
    
    int i = 0;
    t_list *current = grp->argv;
    while (current)
    {
        t_command *cmd = (t_command *)current->content;
        argv[i] = strdup(cmd->arg);
        if (!argv[i])
        {
            while (--i >= 0)
                free(argv[i]);
            free(argv);
            return NULL;
        }
        i++;
        current = current->next;
    }
    argv[i] = NULL;
    return argv;
}

/* Count arguments in list */
int count_args(t_list *args)
{
    int count = 0;
    while (args)
    {
        count++;
        args = args->next;
    }
    return count;
}

/* Free argv array */
void free_argv(char **argv)
{
    if (!argv)
        return;
    for (int i = 0; argv[i]; i++)
        free(argv[i]);
    free(argv);
}

/* Execute all command groups */
int execute_commands(t_list *groups, char **envp, t_info *info)
{
    t_exec_ctx ctx;
    
    /* Initialize execution context */
    ctx.stdin_backup = dup(STDIN_FILENO);
    ctx.stdout_backup = dup(STDOUT_FILENO);
    ctx.last_exit_status = info->last_exit_status;
    ctx.envp = envp;
    ctx.info = info;
    
    /* Handle empty command (just pressing enter) */
    if (!groups)
    {
        close(ctx.stdin_backup);
        close(ctx.stdout_backup);
        return info->last_exit_status;
    }
    
    /* Check if we have pipes by counting groups */
    int group_count = 0;
    t_list *tmp = groups;
    while (tmp)
    {
        group_count++;
        tmp = tmp->next;
    }
    
    /* If multiple groups, use pipeline execution */
    if (group_count > 1)
    {
        ctx.last_exit_status = execute_pipeline(groups, &ctx);
    }
    else if (groups)
    {
        /* Single command execution */
        t_group *grp = (t_group *)groups->content;
        ctx.last_exit_status = execute_single_command(grp, &ctx);
    }
    
    /* Clean up */
    close(ctx.stdin_backup);
    close(ctx.stdout_backup);
    
    /* Update info with last exit status */
    info->last_exit_status = ctx.last_exit_status;
    
    /* Update $? environment variable */
    char exit_str[16];
    snprintf(exit_str, sizeof(exit_str), "%d", ctx.last_exit_status);
    setenv("?", exit_str, 1);
    
    return ctx.last_exit_status;
}

/* Execute a single command group */
int execute_single_command(t_group *grp, t_exec_ctx *ctx)
{
    if (!grp || !grp->argv)
        return 0;
    
    /* Convert group to argv */
    char **argv = group_to_argv(grp);
    if (!argv)
        return 1;
    
    /* Skip if empty command */
    if (!argv[0])
    {
        free_argv(argv);
        return 0;
    }
    
    int status = 0;
    
    /* Backup standard file descriptors */
    int stdin_temp = dup(STDIN_FILENO);
    int stdout_temp = dup(STDOUT_FILENO);
    
    /* Setup redirections */
    if (setup_redirections(&argv) < 0)
    {
        status = 1;
        goto cleanup;
    }
    
    /* Check if command exists after redirections are removed */
    if (!argv[0])
    {
        status = 0;
        goto cleanup;
    }
    
    /* Handle empty command that expands to nothing (like $EMPTY) */
    if (strlen(argv[0]) == 0)
    {
        status = 0;
        goto cleanup;
    }
    
    /* Execute built-in or external command */
    if (is_builtin(argv[0]))
        status = execute_builtin(argv, ctx);
    else
        status = execute_external(argv, ctx);
    
cleanup:
    /* Restore standard file descriptors */
    dup2(stdin_temp, STDIN_FILENO);
    dup2(stdout_temp, STDOUT_FILENO);
    close(stdin_temp);
    close(stdout_temp);
    
    free_argv(argv);
    return status;
}
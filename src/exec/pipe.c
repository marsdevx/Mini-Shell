#include "../init/header.h"

/* Execute commands with pipes */
int execute_pipeline(t_list *groups, t_exec_ctx *ctx)
{
    int pipe_count = 0;
    t_list *tmp = groups;
    
    /* Count commands in pipeline */
    while (tmp)
    {
        pipe_count++;
        tmp = tmp->next;
    }
    
    if (pipe_count == 1)
    {
        /* No pipes, single command */
        t_group *grp = (t_group *)groups->content;
        return execute_single_command(grp, ctx);
    }
    
    /* Allocate pipe array */
    int (*pipes)[2] = malloc(sizeof(int[2]) * (pipe_count - 1));
    if (!pipes)
        return 1;
    
    /* Create all pipes */
    for (int i = 0; i < pipe_count - 1; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            /* Close already created pipes */
            for (int j = 0; j < i; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            free(pipes);
            return 1;
        }
    }
    
    /* Execute each command in pipeline */
    int cmd_index = 0;
    pid_t *pids = malloc(sizeof(pid_t) * pipe_count);
    if (!pids)
    {
        for (int i = 0; i < pipe_count - 1; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        free(pipes);
        return 1;
    }
    
    while (groups)
    {
        t_group *grp = (t_group *)groups->content;
        char **argv = group_to_argv(grp);
        if (!argv)
        {
            /* Clean up on error */
            for (int i = 0; i < cmd_index; i++)
            {
                kill(pids[i], SIGTERM);
            }
            for (int i = 0; i < pipe_count - 1; i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            free(pids);
            free(pipes);
            return 1;
        }
        
        pids[cmd_index] = fork();
        if (pids[cmd_index] < 0)
        {
            perror("fork");
            free_argv(argv);
            /* Clean up */
            for (int i = 0; i < cmd_index; i++)
            {
                kill(pids[i], SIGTERM);
            }
            for (int i = 0; i < pipe_count - 1; i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            free(pids);
            free(pipes);
            return 1;
        }
        else if (pids[cmd_index] == 0)
        {
            /* Child process */
            
            /* Reset signals */
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            
            /* Setup pipe redirections */
            if (cmd_index > 0)
            {
                /* Not first command - read from previous pipe */
                dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
            }
            
            if (cmd_index < pipe_count - 1)
            {
                /* Not last command - write to next pipe */
                dup2(pipes[cmd_index][1], STDOUT_FILENO);
            }
            
            /* Close all pipe fds in child */
            for (int i = 0; i < pipe_count - 1; i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            
            /* Setup file redirections */
            if (setup_redirections(&argv) < 0)
                exit(1);
            
            /* Execute command */
            if (!argv[0])
                exit(0);
                
            if (is_builtin(argv[0]))
            {
                /* Create a copy of context for child process */
                t_exec_ctx child_ctx = *ctx;
                /* Prevent exit builtin from affecting parent's exit flag */
                t_info child_info = *ctx->info;
                child_ctx.info = &child_info;
                
                int status = execute_builtin(argv, &child_ctx);
                exit(status);
            }
            else
            {
                char *cmd_path = resolve_command_path(argv[0]);
                if (!cmd_path)
                {
                    fprintf(stderr, "%s: command not found\n", argv[0]);
                    exit(127);
                }
                execve(cmd_path, argv, ctx->envp);
                perror(argv[0]);
                exit(126);
            }
        }
        
        /* Parent continues */
        free_argv(argv);
        groups = groups->next;
        cmd_index++;
    }
    
    /* Parent: close all pipes */
    for (int i = 0; i < pipe_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    /* Wait for all children */
    int status = 0;
    for (int i = 0; i < pipe_count; i++)
    {
        int child_status;
        if (waitpid(pids[i], &child_status, 0) < 0)
        {
            perror("waitpid");
        }
        /* Keep status of last command */
        if (i == pipe_count - 1)
        {
            if (WIFEXITED(child_status))
                status = WEXITSTATUS(child_status);
            else if (WIFSIGNALED(child_status))
                status = 128 + WTERMSIG(child_status);
        }
    }
    
    free(pids);
    free(pipes);
    return status;
}
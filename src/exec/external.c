#include "../init/header.h"

/* Execute external command */
int execute_external(char **args, t_exec_ctx *ctx)
{
    char *cmd_path = resolve_command_path(args[0]);
    if (!cmd_path)
    {
        fprintf(stderr, "bash: %s: command not found\n", args[0]);
        return 127;
    }
    
    /* Check if file exists but is a directory */
    struct stat st;
    if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "bash: %s: Is a directory\n", args[0]);
        free(cmd_path);
        return 126;
    }
    
    /* Check if file exists but is not executable */
    if (access(cmd_path, X_OK) != 0 && access(cmd_path, F_OK) == 0)
    {
        fprintf(stderr, "bash: %s: Permission denied\n", args[0]);
        free(cmd_path);
        return 126;
    }
    
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        free(cmd_path);
        return 1;
    }
    else if (pid == 0)
    {
        /* Child process */
        
        /* Reset signal handlers */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        
        /* Execute command */
        execve(cmd_path, args, ctx->envp);
        
        /* If we get here, execve failed */
        perror(args[0]);
        exit(126); /* Cannot execute */
    }
    else
    {
        /* Parent process */
        int status;
        
        /* Wait for child */
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            free(cmd_path);
            return 1;
        }
        
        free(cmd_path);
        
        /* Handle exit status */
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            return 128 + WTERMSIG(status);
        else
            return 1;
    }
}
#include "../init/header.h"

void process_input(char *line, t_info *info, char **envp)
{
  if (!line)
    return;

  /* Handle empty line */
  if (*line == '\0')
    return;

  t_list *tokens = lexer(line);
  if (!tokens)
  {
    // Lexer failed - set exit status to 2 (syntax error)
    setenv("?", "2", 1);
    info->last_exit_status = 2;
    return;
  }

  t_list *groups = parser(tokens);
  if (groups)
  {
    int exit_status = execute_commands(groups, envp, info);
    
    // Convert exit status to string and set in environment
    char exit_str[16];
    snprintf(exit_str, sizeof(exit_str), "%d", exit_status);
    setenv("?", exit_str, 1);
    info->last_exit_status = exit_status;
    
    free_groups(&groups);
  }
  else
  {
    // Parser failed - set exit status to 2 (syntax error)
    setenv("?", "2", 1);
    info->last_exit_status = 2;
  }

  ft_free_tokens(&tokens);
}

int main(int argc, char *argv[], char *envp[])
{
    t_info info;
    char *line;

    (void)argc;
    (void)argv;

    if (ft_init(&info) != 0) {
        return EXIT_FAILURE;
    }

    // Initialize exit status to 0
    setenv("?", "0", 1);
    info.last_exit_status = 0;
    
    // Ensure USER is set if not present
    if (!getenv("USER"))
    {
        char *user = getenv("LOGNAME");
        if (user)
            setenv("USER", user, 1);
        else
            setenv("USER", "user", 1);
    }
    
    // Ensure HOME is set if not present
    if (!getenv("HOME"))
    {
        char *home = getenv("HOME");
        if (!home)
            setenv("HOME", "/home/user", 1);
    }

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);

    while (info.exit_f) {
        line = ft_readline("minishell> ");
        if (line == NULL) {
            printf("exit\n");
            break;
        }
        if (*line) {
            process_input(line, &info, envp);
        }
        free(line);
    }

    return info.last_exit_status;
}

// static void ensure_env_defaults(void)
// {
//   if (!getenv("USER"))
//   {
//     const char *user = getenv("LOGNAME");
//     setenv("USER", user ? user : "user", 1);
//   }

//   if (!getenv("HOME"))
//   {
//     setenv("HOME", "/home/user", 1);
//   }
// }

// int main(int argc, char *argv[], char *envp[])
// {
//   t_info info;
//   char *line = NULL;

//   if (ft_init(&info) != 0)
//     return EXIT_FAILURE;

//   setenv("?", "0", 1);
//   info.last_exit_status = 0;

//   ensure_env_defaults();

//   signal(SIGINT, handle_sigint);
//   signal(SIGQUIT, SIG_IGN);

//   if (argc > 2 && strcmp(argv[1], "-c") == 0)
//   {
//     line = strdup(argv[2]);
//     if (!line)
//       return EXIT_FAILURE;
//     process_input(line, &info, envp);
//     free(line);
//   }
//   else if (argc > 1)
//   {
//     fprintf(stderr, "minishell: %s: command not found\n", argv[1]);
//     return 127;
//   }
//   else
//   {
//     while (info.exit_f)
//     {
//       line = ft_readline("minishell> ");
//       if (!line)
//       {
//         printf("exit\n");
//         break;
//       }
//       if (*line)
//         process_input(line, &info, envp);
//       free(line);
//     }
//   }

//   return info.last_exit_status;
// }
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

int main(void)
{
    char *line;

    while (1)
    {
        // Read input with a prompt
        line = readline("minishell> ");
        
        // Check for EOF (Ctrl+D)
        if (line == NULL)
        {
            break; // Exit the loop to terminate the shell
        }
        
        // Add non-empty input to history
        if (*line) // Check if the string is not empty
        {
            add_history(line);
        }
        
        // Process the command here (e.g., parse and execute)
        // For now, we’ll just assume this part is handled elsewhere
        // Example: printf("You entered: %s\n", line);
        
        // Free the allocated memory
        free(line);
    }
    return (0);
}
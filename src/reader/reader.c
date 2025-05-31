
#include "../header/header.h"

static char	*ft_readline(void)
{
	char	*line;

	line = readline("minishell> ");
	if (line && *line)
		add_history(line);
	return (line);
}

//gcc -Wall -Wextra -g3 /home/dkot/Desktop/Mini-Shell/src/reader/reader.c -o /home/dkot/Desktop/Mini-Shell/src/reader/output/reader -lreadline -lncurses
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

        free(line);
    }
    return (0);
}
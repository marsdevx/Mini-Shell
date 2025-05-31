#include "../header/header.h"

/* Helper function: Duplicate a string up to n characters */
char *ft_strndup(const char *s, size_t n)
{
    char *dup = malloc(n + 1);
    if (!dup)
        return NULL;
    strncpy(dup, s, n);
    dup[n] = '\0';
    return dup;
}

/* Check for unmatched quotes in the input */
int quotes_check(char *input)
{
    int i = 0;
    int in_single_quote = 0;
    int in_double_quote = 0;

    while (input[i])
    {
        if (input[i] == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote;
        else if (input[i] == '"' && !in_single_quote)
            in_double_quote = !in_double_quote;
        i++;
    }
    if (in_single_quote || in_double_quote)
    {
        printf("Error: Unmatched quotes\n");
        return 0;
    }
    return 1;
}

/* Lexer function: Tokenize the input string */
t_list *lexer(char *input)
{
    if (!quotes_check(input))
        return NULL;

    t_list *tokens = NULL;
    char *ptr = input;

    while (*ptr)
    {
        char *start = ptr;
        t_token *token = malloc(sizeof(t_token));
        if (!token)
            return NULL;

        /* Handle separators (spaces/tabs) */
        if (*ptr == ' ' || *ptr == '\t')
        {
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;
            size_t len = ptr - start;
            token->type = SEP;
            token->value = ft_strndup(start, len);
            token->value_len = len;
        }
        /* Handle pipe symbol */
        else if (*ptr == '|')
        {
            token->type = PIPE;
            token->value = ft_strndup(ptr, 1);
            token->value_len = 1;
            ptr++;
        }
        /* Handle input redirection or heredoc */
        else if (*ptr == '<')
        {
            if (*(ptr + 1) == '<')
            {
                token->type = HEREDOC;
                token->value = ft_strndup(ptr, 2);
                token->value_len = 2;
                ptr += 2;
            }
            else
            {
                token->type = REDIRECT_IN;
                token->value = ft_strndup(ptr, 1);
                token->value_len = 1;
                ptr++;
            }
        }
        /* Handle output redirection or append */
        else if (*ptr == '>')
        {
            if (*(ptr + 1) == '>')
            {
                token->type = REDIRECT_APPEND;
                token->value = ft_strndup(ptr, 2);
                token->value_len = 2;
                ptr += 2;
            }
            else
            {
                token->type = REDIRECT_OUT;
                token->value = ft_strndup(ptr, 1);
                token->value_len = 1;
                ptr++;
            }
        }
        /* Handle single-quoted fields */
        else if (*ptr == '\'')
        {
            ptr++; // Skip opening quote
            start = ptr;
            while (*ptr && *ptr != '\'')
                ptr++;
            if (*ptr == '\'') // Found closing quote
            {
                size_t len = ptr - start;
                token->type = FIELD;
                token->value = ft_strndup(start, len);
                token->value_len = len;
                ptr++; // Skip closing quote
            }
            else
            {
                free(token);
                return NULL; // Unmatched quote (caught by quotes_check)
            }
        }
        /* Handle double-quoted fields */
        else if (*ptr == '"')
        {
            ptr++; // Skip opening double quote
            start = ptr;
            while (*ptr && *ptr != '"')
                ptr++;
            if (*ptr == '"') // Found closing double quote
            {
                size_t len = ptr - start;
                token->type = EXP_FIELD;
                token->value = ft_strndup(start, len);
                token->value_len = len;
                ptr++; // Skip closing double quote
            }
            else
            {
                free(token);
                return NULL; // Unmatched quote (caught by quotes_check)
            }
        }
        /* Handle unquoted words */
        else
        {
            while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '|' && *ptr != '<' && *ptr != '>' && *ptr != '\'' && *ptr != '"')
                ptr++;
            size_t len = ptr - start;
            token->type = WORD;
            token->value = ft_strndup(start, len);
            token->value_len = len;
        }

        ft_lstadd_back(&tokens, ft_lstnew(token));
        if (*ptr == '\0')
            break;
    }
    return tokens;
}




// /* Main function for testing */
// int main(void)
// {
//     char *input = "echo \"dshgjksfgsf'''rgfsdggj42tc|\" >> file";
//     printf("Testing input: %s\n", input);
//     t_list *tokens = lexer(input);
//     if (tokens)
//     {
//         print_tokens(tokens);
//         ft_free_tokens(&tokens);
//     }
//     return 0;
// }
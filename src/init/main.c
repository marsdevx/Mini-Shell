#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

typedef struct s_list {
    void *content;
    struct s_list *next;
} t_list;

typedef enum e_token_type {
    SEP,
    PIPE,
    REDIRECT_IN,
    REDIRECT_OUT,
    REDIRECT_APPEND,
    HEREDOC,
    FIELD,
    EXP_FIELD,
    WORD
} t_token_type;

typedef struct s_token {
    t_token_type type;
    char *value;
    size_t value_len;
} t_token;

typedef struct s_info {
    int exit_f;
} t_info;

char *ft_strndup(const char *s, size_t n)
{
    char *dup = malloc(n + 1);
    if (!dup)
        return NULL;
    strncpy(dup, s, n);
    dup[n] = '\0';
    return dup;
}

t_list *ft_lstnew(void *content)
{
    t_list *node = malloc(sizeof(t_list));
    if (!node)
        return NULL;
    node->content = content;
    node->next = NULL;
    return node;
}

void ft_lstadd_back(t_list **lst, t_list *new)
{
    if (!lst || !new)
        return;
    if (!*lst)
        *lst = new;
    else
    {
        t_list *last = *lst;
        while (last->next)
            last = last->next;
        last->next = new;
    }
}

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

        if (*ptr == ' ' || *ptr == '\t')
        {
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;
            token->type = SEP;
            token->value = ft_strndup(ptr - 1, 1);
            token->value_len = 1;
        }
        else if (*ptr == '|')
        {
            token->type = PIPE;
            token->value = ft_strndup(ptr, 1);
            token->value_len = 1;
            ptr++;
        }
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
        else if (*ptr == '\'')
        {
            ptr++;
            start = ptr;
            while (*ptr && *ptr != '\'')
                ptr++;
            if (*ptr == '\'')
            {
                size_t len = ptr - start;
                token->type = FIELD;
                token->value = ft_strndup(start, len);
                token->value_len = len;
                ptr++;
            }
            else
            {
                free(token);
                return NULL;
            }
        }
        else if (*ptr == '"')
        {
            ptr++;
            start = ptr;
            while (*ptr && *ptr != '"')
                ptr++;
            if (*ptr == '"')
            {
                size_t len = ptr - start;
                token->type = EXP_FIELD;
                token->value = ft_strndup(start, len);
                token->value_len = len;
                ptr++;
            }
            else
            {
                free(token);
                return NULL;
            }
        }
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

void print_tokens(t_list *tokens)
{
    t_list *current = tokens;
    while (current)
    {
        t_token *token = (t_token *)current->content;
        printf("Type: %d, Value: %s, Length: %zu\n", token->type, token->value, token->value_len);
        current = current->next;
    }
}

void free_tokens(t_list **tokens)
{
    t_list *current = *tokens;
    while (current)
    {
        t_list *next = current->next;
        t_token *token = (t_token *)current->content;
        free(token->value);
        free(token);
        free(current);
        current = next;
    }
    *tokens = NULL;
}

static int ft_init(t_info *info)
{
    info->exit_f = 1;
    return 0;
}

static void handle_sigint(int sig)
{
    if (sig == SIGINT) {
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

static char *ft_readline(const char *prompt)
{
    char *line = readline(prompt);
    if (line && *line) {
        add_history(line);
    }
    return line;
}

static void process_input(char *line, t_info *info)
{
    t_list *tokens = lexer(line);
    if (tokens) {
        print_tokens(tokens);
        free_tokens(&tokens);
    }
}

int main(int argc, char *argv[], char *envp[])
{
    t_info info;
    char *line;

    (void)argc;
    (void)argv;
    (void)envp;

    if (ft_init(&info) != 0) {
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);

    while (1) {
        line = ft_readline("minishell> ");
        if (line == NULL) {
            printf("exit\n");
            break;
        }
        if (*line) {
            process_input(line, &info);
        }
        free(line);
    }

    return 0;
}
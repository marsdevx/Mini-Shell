Dollar handle
Several words without separator handle
No word after redirect handle


output -> process -> input

# What is output
  - groups
    - each groups it's all tokens in between two pipes
  - commands
  - information

# Input 
  - token list



linked list inside linked list

linked list - groups (content: commands)
linked list - command (content: information)

- i need to store my own envp list, or change existing?


group {
  t_cmd command;
  struct group *next;
}

t_cmd {
  char *arg:
  char *files;
  strcut t_cmd *next;
}










# Request
вот стуктура - 
typedef struct s_token
{
    e_type type;      // Token type
    char *value;      // Token value as a string
    int   value_len;  // Length of the value
} t_token;

для того что бы сделать linked list из нее я использовал другую структуру
typedef struct s_list
{
    void *content;         // Pointer to token
    struct s_list *next;   // Next node
} t_list;
и по итогу первая становиться линкед листом, почему это работает могу обьяснить в коде, вот мой код который позволяет этому работать -
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Enum for token types */
typedef enum e_type
{
    WORD,           // Regular words
    FIELD,          // Quoted fields (e.g., 'text')
    EXP_FIELD,      // Expandable fields (e.g., "text")
    SEP,            // Separators (spaces/tabs)
    PIPE,           // Pipe symbol (|)
    REDIRECT_IN,    // Input redirection (<)
    REDIRECT_OUT,   // Output redirection (>)
    REDIRECT_APPEND,// Append redirection (>>)
    HEREDOC         // Heredoc redirection (<<)
} e_type;

/* Structure for a token */
typedef struct s_token
{
    e_type type;      // Token type
    char *value;      // Token value as a string
    int   value_len;  // Length of the value
} t_token;

/* Structure for a linked list node */
typedef struct s_list
{
    void *content;         // Pointer to token
    struct s_list *next;   // Next node
} t_list;

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

/* Helper function: Create a new linked list node */
t_list *ft_lstnew(void *content)
{
    t_list *node = malloc(sizeof(t_list));
    if (!node)
        return NULL;
    node->content = content;
    node->next = NULL;
    return node;
}

/* Helper function: Add a node to the back of the list */
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

/* Free the token list and its contents */
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

/* Print the tokens for debugging */
void print_tokens(t_list *tokens)
{
    t_list *current = tokens;
    while (current)
    {
        t_token *token = (t_token *)current->content;
        printf("Type: %d, Value: %s, Length: %d\n", token->type, token->value, token->value_len);
        current = current->next;
    }
}

/* Main function for testing */
int main(void)
{
    char *input = "echo \"dshgjksfgsf'''rgfsdggj42tc|\" >> file";
    printf("Testing input: %s\n", input);
    t_list *tokens = lexer(input);
    if (tokens)
    {
        print_tokens(tokens);
        free_tokens(&tokens);
    }
    return 0;
}

----
теперь я хочу что бы таже система с созданием линкед листа с помощью вот этой структуры (
typedef struct s_list
{
    void          *content;  /* points to a t_token             */
    struct s_list *next;
} t_list;
) и доп кода работала и в случае двумерного линкед листа - 
а конкретно вот этого 
typedef struct s_command
{
  char  *arg;
  struct s_command  *next;
} t_command;

typedef struct s_group
{
  t_command *command;
  struct s_group   *next;
} t_group;

сделай это






# REq 2


выведи мне код в одном блоке, так же у меня есть заглушка для лексера -
/* ─────────────────────────  data structures  ────────────────────────────── */

typedef enum e_type
{
    WORD,           // Regular words
    FIELD,          // Quoted fields (e.g., 'text')
    EXP_FIELD,      // Expandable fields (e.g., "text")
    SEP,            // Separators (spaces/tabs)
    PIPE,           // Pipe symbol (|)
    REDIRECT_IN,    // Input redirection (<)
    REDIRECT_OUT,   // Output redirection (>)
    REDIRECT_APPEND,// Append redirection (>>)
    HEREDOC         // Heredoc redirection (<<)
} e_type;

typedef struct s_token
{
    e_type type;     /* token class                            */
    char   *value;   /* raw text (NUL-terminated)              */
    int     len;     /* strlen(value) to avoid recalculations  */
} t_token;

typedef struct s_list
{
    void          *content;  /* points to a t_token             */
    struct s_list *next;
} t_list;

/* ───────────────────────  tiny list helpers (no libft)  ─────────────────── */

static t_list *lstnew(void *content)
{
    t_list *n = malloc(sizeof(*n));
    if (!n)
        return NULL;
    n->content = content;
    n->next    = NULL;
    return n;
}

static void lstadd_back(t_list **lst, t_list *new)
{
    if (!lst || !new)
        return;
    if (!*lst)
        *lst = new;
    else
    {
        t_list *cur = *lst;
        while (cur->next)
            cur = cur->next;
        cur->next = new;
    }
}

/* convenience wrapper to allocate + push a token in one call */
static int push_token(t_list **lst, e_type type, const char *str)
{
    t_token *tok = malloc(sizeof(*tok));
    if (!tok)
        return 0;
    tok->type = type;
    tok->len  = (int)strlen(str);
    tok->value = strdup(str);          /* strdup is fine for a demo */
    if (!tok->value)
        return free(tok), 0;
    lstadd_back(lst, lstnew(tok));
    return 1;
}

/* stringify the enum, useful for readable debug output */
static const char *type_name(e_type t)
{
    static const char *names[] = {
        "WORD", "FIELD", "EXP_FIELD", "SEP",
        "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
        "REDIRECT_APPEND", "HEREDOC"
    };
    return names[t];
}

/* dump the linked list */
static void print_tokens(t_list *lst)
{
    for (; lst; lst = lst->next)
    {
        t_token *tk = lst->content;
        printf("%-14s  \"%s\"  (len=%d)\n", type_name(tk->type), tk->value, tk->len);
    }
}

/* free everything */
static void free_tokens(t_list **lst)
{
    while (*lst)
    {
        t_list  *n  = *lst;
        t_token *tk = n->content;
        free(tk->value);
        free(tk);
        *lst = n->next;
        free(n);
    }
}

/* ─────────────────────────────  driver  ─────────────────────────────────── */

int main(void)
{
    /* target command: echo hello | echo world */
    t_list *tokens = NULL;

    /* WORD "echo"         */ if (!push_token(&tokens, WORD, "echo"))          return 1;
    /* SEP  " "            */ if (!push_token(&tokens, SEP,  " "))             return 1;
    /* WORD "hello"        */ if (!push_token(&tokens, WORD, "hello"))         return 1;
    /* SEP  " "            */ if (!push_token(&tokens, SEP,  " "))             return 1;
    /* PIPE "|"            */ if (!push_token(&tokens, PIPE, "|"))             return 1;
    /* SEP  " "            */ if (!push_token(&tokens, SEP,  " "))             return 1;
    /* WORD "echo"         */ if (!push_token(&tokens, WORD, "echo"))          return 1;
    /* SEP  " "            */ if (!push_token(&tokens, SEP,  " "))             return 1;
    /* WORD "world"        */ if (!push_token(&tokens, WORD, "world"))         return 1;

    puts("Token list for:  echo hello | echo world\n");
    print_tokens(tokens);

    free_tokens(&tokens);
    return 0;
}


пускай основная парсер команда примет этот список токенов и из него создаст в этом двумерном линкед листе две команды в первом будут аргументы ()
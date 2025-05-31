#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


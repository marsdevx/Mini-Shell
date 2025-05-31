#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─────────────────────────  data structures  ────────────────────────────── */

typedef enum e_type
{
    WORD, FIELD, EXP_FIELD, SEP, PIPE,
    REDIRECT_IN, REDIRECT_OUT, REDIRECT_APPEND, HEREDOC
} e_type;

typedef struct s_token
{
    e_type type;   /* token class                           */
    char  *value;  /* raw text (NUL-terminated)             */
    int    len;    /* strlen(value)                         */
} t_token;

/* универсальный узел связного списка (используем два уровня) */
typedef struct s_list
{
    void          *content;  /* void* → t_token, t_group, t_command … */
    struct s_list *next;
} t_list;

/* второй уровень: одиночный аргумент команды */
typedef struct s_command { char *arg; } t_command;

/* первый уровень: сама команда (т.е. всё между PIPE) */
typedef struct s_group  { t_list *argv; } t_group;

/* ───────────────────────────  globals  ──────────────────────────────────── */

/* token-name lookup table (was “static const” inside type_name) */
const char *const g_type_name[] = {
    "WORD", "FIELD", "EXP_FIELD", "SEP",
    "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
    "REDIRECT_APPEND", "HEREDOC"
};

/* ── redirect classifier ─────────────────────────────────────────────── */
static int is_redirect(e_type t)
{
    return (t == REDIRECT_IN
         || t == REDIRECT_OUT
         || t == REDIRECT_APPEND
         || t == HEREDOC);
}

/* forward declaration – needed because tokens_to_groups() calls it */
void free_groups(t_list **groups);

/* ───────────────────────  tiny list helpers (no libft)  ─────────────────── */

t_list *lstnew(void *content)
{
    t_list *n = malloc(sizeof(*n));
    if (!n)
        return NULL;
    n->content = content;
    n->next    = NULL;
    return n;
}

void lstadd_back(t_list **lst, t_list *new)
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

/* ─────────────────────────  token helpers  ─────────────────────────────── */

int push_token(t_list **lst, e_type type, const char *str)
{
    t_token *tok = malloc(sizeof(*tok));
    if (!tok)
        return 0;
    tok->type  = type;
    tok->len   = (int)strlen(str);
    tok->value = strdup(str);
    if (!tok->value)
        return free(tok), 0;
    lstadd_back(lst, lstnew(tok));
    return 1;
}

const char *type_name(e_type t)
{
    return g_type_name[t];
}

void print_tokens(t_list *lst)
{
    for (; lst; lst = lst->next)
    {
        t_token *tk = lst->content;
        printf("%-14s  \"%s\"  (len=%d)\n",
               type_name(tk->type), tk->value, tk->len);
    }
}

void free_tokens(t_list **lst)
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

/* ──────────────────── helpers for 2-D list (groups / argv) ─────────────── */

t_command *new_command(const char *arg)
{
    t_command *cmd = malloc(sizeof(*cmd));
    if (!cmd)
        return NULL;
    cmd->arg = strdup(arg);
    if (!cmd->arg)
        return free(cmd), NULL;
    return cmd;
}

t_group *new_group(void)
{
    t_group *grp = malloc(sizeof(*grp));
    if (!grp)
        return NULL;
    grp->argv = NULL;
    return grp;
}

int add_argument(t_group *grp, const char *arg)
{
    t_command *cmd = new_command(arg);
    if (!cmd)
        return 0;
    t_list *node = lstnew(cmd);
    if (!node)
        return free(cmd->arg), free(cmd), 0;
    lstadd_back(&grp->argv, node);
    return 1;
}

/* ────────────────────────  PARSER (1-to-2-D)  ──────────────────────────── */

t_list *tokens_to_groups(t_list *tok_lst)
{
    t_list  *groups = NULL;   /* outer list (pipeline)      */
    t_group *cur    = NULL;   /* current command segment    */

    while (tok_lst)
    {
        t_token *tk = tok_lst->content;

        /* ── NEW: validate redirect syntax ───────────────────────────── */
        if (is_redirect(tk->type))
        {
            t_list *look = tok_lst->next;                /* token after >, >>, <, << */

            /* optional single space that your lexer still keeps            */
            if (look && ((t_token *)look->content)->type == SEP)
                look = look->next;                       /* skip that one SEP */

            /* missing token, or next thing is NOT a WORD → error           */
            if (!look || ((t_token *)look->content)->type != WORD)
            {
                fprintf(stderr,
                        "syntax error: redirect \"%s\" needs a filename\n",
                        tk->value);
                free_groups(&groups);                    /* prevent leaks    */
                return NULL;                             /* signal failure   */
            }
        }
        /* ─────────────────────────────────────────────────────────────── */

        if (tk->type == PIPE)                   /* new pipeline segment   */
            cur = NULL;
        else if (tk->type != SEP)               /* WORD / FIELD / …       */
        {
            if (!cur)                           /* first arg in segment?  */
            {
                cur = new_group();
                if (!cur) { free_groups(&groups); return NULL; }
                lstadd_back(&groups, lstnew(cur));
            }
            if (!add_argument(cur, tk->value))
            {
                free_groups(&groups);
                return NULL;
            }
        }
        tok_lst = tok_lst->next;
    }
    return groups;
}

/* ─────────────────────── debug & free for 2-D list ─────────────────────── */

void print_groups(t_list *groups)
{
    int g = 0;
    for (; groups; groups = groups->next, ++g)
    {
        t_group *grp = groups->content;
        printf("Command %d:\n", g);
        int a = 0;
        for (t_list *arg = grp->argv; arg; arg = arg->next, ++a)
            printf("   arg[%d] = \"%s\"\n",
                   a, ((t_command *)arg->content)->arg);
    }
}

void free_groups(t_list **groups)
{
    while (*groups)
    {
        t_list  *g_next = (*groups)->next;
        t_group *grp    = (*groups)->content;

        while (grp->argv)
        {
            t_list     *a_next = grp->argv->next;
            t_command  *cmd    = grp->argv->content;
            free(cmd->arg);
            free(cmd);
            free(grp->argv);
            grp->argv = a_next;
        }
        free(grp);
        free(*groups);
        *groups = g_next;
    }
}

/* ─────────────────────────────  driver  ─────────────────────────────────── */

int main(void)
{
    /* target command: echo hello | echo world */
    t_list *tokens = NULL;

    /* съёмка токенов-заглушек вместо полноценного лексера */
    

    puts("── token list ─────────────────────────────");
    print_tokens(tokens);

    /* 1-D → 2-D */
    t_list *groups = tokens_to_groups(tokens);
    if (!groups)
    {
        perror("parser");
        free_tokens(&tokens);
        return 1;
    }

    puts("\n── parsed commands ───────────────────────");
    print_groups(groups);

    /* cleanup */
    free_groups(&groups);
    free_tokens(&tokens);
    return 0;
}



// if (!push_token(&tokens, WORD, "echo"))   return 1;
// if (!push_token(&tokens, SEP,  " "))      return 1;
// if (!push_token(&tokens, WORD, "hello"))  return 1;
// if (!push_token(&tokens, SEP,  " "))      return 1;
// if (!push_token(&tokens, PIPE, "|"))      return 1;
// if (!push_token(&tokens, SEP,  " "))      return 1;
// if (!push_token(&tokens, WORD, "echo"))   return 1;
// if (!push_token(&tokens, SEP,  " "))      return 1;
// if (!push_token(&tokens, WORD, "world"))  return 1;


// push_token(&tokens, WORD, "ls");
// push_token(&tokens, SEP , " ");
// push_token(&tokens, WORD, "-la");
// push_token(&tokens, SEP , " ");
// push_token(&tokens, WORD, "/usr/bin");


// push_token(&tokens, WORD, "grep");         push_token(&tokens, SEP, " ");
// push_token(&tokens, WORD, "-i");           push_token(&tokens, SEP, " ");
// push_token(&tokens, WORD, "pattern");      push_token(&tokens, SEP, " ");
// push_token(&tokens, REDIRECT_IN,  "<");    push_token(&tokens, SEP, " ");
// push_token(&tokens, WORD, "in.txt");       push_token(&tokens, SEP, " ");
// push_token(&tokens, REDIRECT_OUT, ">");    push_token(&tokens, SEP, " ");
// push_token(&tokens, WORD, "out.txt");

// push_token(&tokens, WORD, "grep");         push_token(&tokens, SEP, " ");
// push_token(&tokens, WORD, "-i");           push_token(&tokens, SEP, " ");
// push_token(&tokens, WORD, "pattern");      push_token(&tokens, SEP, " ");
// push_token(&tokens, REDIRECT_IN,  "<");    push_token(&tokens, SEP, " ");

// push_token(&tokens, HEREDOC,  "<");    push_token(&tokens, SEP, " ");
// push_token(&tokens, REDIRECT_OUT,  "<");    push_token(&tokens, SEP, " ");
// push_token(&tokens, REDIRECT_APPEND,  "<");    push_token(&tokens, SEP, " ");

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
    t_list  *groups = NULL;   /* верхний уровень */
    t_group *cur    = NULL;   /* текущая команда */

    while (tok_lst)
    {
        t_token *tk = tok_lst->content;

        if (tk->type == PIPE)               /* новый блок команд */
            cur = NULL;
        else if (tk->type != SEP)           /* WORD / FIELD / EXP_FIELD */
        {
            if (!cur)                       /* начала новой команды нет? */
            {
                cur = new_group();
                if (!cur)
                    return NULL;
                lstadd_back(&groups, lstnew(cur));
            }
            if (!add_argument(cur, tk->value))
                return NULL;
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
    if (!push_token(&tokens, WORD, "echo"))   return 1;
    if (!push_token(&tokens, SEP,  " "))      return 1;
    if (!push_token(&tokens, WORD, "hello"))  return 1;
    if (!push_token(&tokens, SEP,  " "))      return 1;
    if (!push_token(&tokens, PIPE, "|"))      return 1;
    if (!push_token(&tokens, SEP,  " "))      return 1;
    if (!push_token(&tokens, WORD, "echo"))   return 1;
    if (!push_token(&tokens, SEP,  " "))      return 1;
    if (!push_token(&tokens, WORD, "world"))  return 1;

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
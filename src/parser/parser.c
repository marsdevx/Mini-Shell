#include "../header/header.h"

/* ───────────────────────────  globals  ──────────────────────────────────── */

const char *const g_type_name[] = {
    "WORD", "FIELD", "EXP_FIELD", "SEP",
    "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
    "REDIRECT_APPEND", "HEREDOC"
};

/* ── small helpers ──────────────────────────────────────────────────────── */

static int is_redirect(e_type t)
{
    return (t == REDIRECT_IN || t == REDIRECT_OUT ||
            t == REDIRECT_APPEND || t == HEREDOC);
}

static int is_text(e_type t)
{
    return (t == WORD || t == FIELD || t == EXP_FIELD);
}

/* Expand $VARNAME inside an EXP_FIELD token (simple version) */
static char *expand_env(const char *src)
{
    const char *p = src;
    size_t  cap = strlen(src) + 32;
    size_t  len = 0;
    char   *out = malloc(cap);
    if (!out) return NULL;

    while (*p)
    {
        if (*p == '$' && isalpha((unsigned char)p[1]))
        {
            const char *v = ++p;
            while (isalnum((unsigned char)*p) || *p == '_')
                ++p;
            size_t vlen = p - v;
            char var[256];
            if (vlen >= sizeof var) vlen = sizeof var - 1;
            memcpy(var, v, vlen);
            var[vlen] = '\0';

            const char *val = getenv(var);
            if (!val) val = "";

            size_t need = len + strlen(val) + 1;
            if (need > cap) { cap = need * 2; out = realloc(out, cap); if (!out) return NULL; }
            strcpy(out + len, val);
            len += strlen(val);
        }
        else
        {
            if (len + 2 > cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
            out[len++] = *p++;
            out[len]   = '\0';
        }
    }
    return out;
}

/* forward declaration */
void free_groups(t_list **groups);

/* ────────────────── token helpers ─────────────────────────────────────── */

int push_token(t_list **lst, e_type type, const char *str)
{
    t_token *tok = malloc(sizeof *tok);
    if (!tok) return 0;
    tok->type  = type;
    tok->value_len   = (int)strlen(str);
    tok->value = strdup(str);
    if (!tok->value) { free(tok); return 0; }
    ft_lstadd_back(lst, ft_lstnew(tok));
    return 1;
}

/* for debug */
void ft_print_tokens(t_list *lst)
{
    for (; lst; lst = lst->next)
    {
        t_token *tk = lst->content;
        printf("%-14s \"%s\" (len=%d)\n", g_type_name[tk->type], tk->value, tk->value_len);
    }
}

const char *type_name(e_type t)
{
    return g_type_name[t];
}

void ft_free_tokens(t_list **lst)
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

/* ─────────────── higher-level list helpers ────────────────────────────── */

t_command *new_command(const char *arg)
{
    t_command *c = malloc(sizeof *c);
    if (!c) return NULL;
    c->arg = strdup(arg);
    if (!c->arg) { free(c); return NULL; }
    return c;
}

t_group *new_group(void)
{
    t_group *g = malloc(sizeof *g);
    if (!g) return NULL;
    g->argv = NULL;
    return g;
}

int add_argument(t_group *grp, const char *arg)
{
    t_command *cmd = new_command(arg);
    if (!cmd) return 0;
    ft_lstadd_back(&grp->argv, ft_lstnew(cmd));
    return 1;
}

/* ──────────────────── parser 1-D → 2-D ────────────────────────────────── */

/* ──────────────────── parser 1-D → 2-D ─────────────────────────────── */

t_list *tokens_to_groups(t_list *tok_lst)
{
    t_list  *groups = NULL;
    t_group *cur    = NULL;

    while (tok_lst)
    {
        t_token *tk = tok_lst->content;

        /* ---------- 1. redirect handling (+ syntax guard) ------------- */
        if (is_redirect(tk->type))
        {
            /* a) verify there is a filename after the redirect --------- */
            t_list *look = tok_lst->next;
            if (look && ((t_token *)look->content)->type == SEP)
                look = look->next;                     /* optional space */
            if (!look || !is_text(((t_token *)look->content)->type))
            {
                fprintf(stderr,
                        "syntax error: redirect \"%s\" needs a filename\n",
                        tk->value);
                free_groups(&groups);
                return NULL;
            }

            /* b) start a command node if we haven't yet ---------------- */
            if (!cur)
            {
                cur = new_group();
                if (!cur) { free_groups(&groups); return NULL; }
                ft_lstadd_back(&groups, ft_lstnew(cur));
            }

            /* c) store the redirect operator as an argument ------------ */
            if (!add_argument(cur, tk->value))
            {   free_groups(&groups); return NULL; }

            /* d) advance to next token and continue the main loop ------ */
            tok_lst = tok_lst->next;
            continue;
        }

        /* ---------- 2. pipe → new command ----------------------------- */
        if (tk->type == PIPE) { cur = NULL; tok_lst = tok_lst->next; continue; }

        /* ---------- 3. skip separators -------------------------------- */
        if (tk->type == SEP)   { tok_lst = tok_lst->next; continue; }

        /* ---------- 4. merge & expand WORD / FIELD / EXP_FIELD -------- */
        if (is_text(tk->type))
        {
            if (!cur)
            {
                cur = new_group();
                if (!cur) { free_groups(&groups); return NULL; }
                ft_lstadd_back(&groups, ft_lstnew(cur));
            }

            char   *arg  = NULL;
            size_t  len  = 0;
            t_list *scan = tok_lst;

            while (scan && is_text(((t_token *)scan->content)->type))
            {
                t_token *tk2  = scan->content;
                char    *piece = (tk2->type == EXP_FIELD && tk2->value[0] == '$')
                                   ? expand_env(tk2->value)
                                   : strdup(tk2->value);
                if (!piece) { free_groups(&groups); free(arg); return NULL; }

                size_t plen = strlen(piece);
                char *tmp   = realloc(arg, len + plen + 1);
                if (!tmp)   { free_groups(&groups); free(arg); free(piece); return NULL; }
                arg = tmp;
                memcpy(arg + len, piece, plen);
                len += plen;
                arg[len] = '\0';
                free(piece);
                scan = scan->next;
            }

            if (!add_argument(cur, arg))
            {   free_groups(&groups); free(arg); return NULL; }
            free(arg);
            tok_lst = scan;
            continue;
        }

        /* ---------- 5. any other token class (nop) -------------------- */
        tok_lst = tok_lst->next;
    }
    return groups;
}

/* ─────────────── debug & cleanup helpers ─────────────────────────────── */

void print_groups(t_list *groups)
{
    int g = 0;
    for (; groups; groups = groups->next, ++g)
    {
        printf("Command %d:\n", g);
        int a = 0;
        for (t_list *arg = ((t_group *)groups->content)->argv; arg; arg = arg->next, ++a)
            printf("  arg[%d] = \"%s\"\n", a, ((t_command *)arg->content)->arg);
    }
}

void free_groups(t_list **groups)
{
    while (*groups)
    {
        t_list *nextg = (*groups)->next;
        t_group *grp  = (*groups)->content;
        while (grp->argv)
        {
            t_list *nexta = grp->argv->next;
            free(((t_command *)grp->argv->content)->arg);
            free(grp->argv->content);
            free(grp->argv);
            grp->argv = nexta;
        }
        free(grp);
        free(*groups);
        *groups = nextg;
    }
}

/* ─────────────────────────────  driver  ─────────────────────────────────── */

// int main(void)
// {
//     /* target command: echo hello | echo world */
//     t_list *tokens = NULL;

//     push_token(&tokens, WORD, "grep");        
//     push_token(&tokens, SEP, " ");
//     push_token(&tokens, WORD, "-i");          
//     push_token(&tokens, SEP, " ");
//     push_token(&tokens, WORD, "pattern");     
//     push_token(&tokens, SEP, " ");
//     push_token(&tokens, REDIRECT_IN,  "<");   
//     push_token(&tokens, SEP, " ");
//     push_token(&tokens, WORD, "in.txt");      
//     push_token(&tokens, SEP, " ");
//     push_token(&tokens, REDIRECT_OUT, ">");   
//     push_token(&tokens, SEP, " ");
//     push_token(&tokens, WORD, "out.txt");



//     puts("── token list ─────────────────────────────");
//     print_tokens(tokens);

//     /* 1-D → 2-D */
//     t_list *groups = tokens_to_groups(tokens);
//     if (!groups)
//     {
//         perror("parser");
//         free_tokens(&tokens);
//         return 1;
//     }

//     puts("\n── parsed commands ───────────────────────");
//     print_groups(groups);

//     /* cleanup */
//     free_groups(&groups);
//     free_tokens(&tokens);
//     return 0;
// }



//     // if (!push_token(&tokens, WORD, "echo"))   return 1;
//     // if (!push_token(&tokens, SEP,  " "))      return 1;
//     // if (!push_token(&tokens, WORD, "hello"))  return 1;
//     // if (!push_token(&tokens, SEP,  " "))      return 1;
//     // if (!push_token(&tokens, PIPE, "|"))      return 1;
//     // if (!push_token(&tokens, SEP,  " "))      return 1;
//     // if (!push_token(&tokens, WORD, "echo"))   return 1;
//     // if (!push_token(&tokens, SEP,  " "))      return 1;
//     // if (!push_token(&tokens, WORD, "world"))  return 1;


//     // push_token(&tokens, WORD, "ls");
//     // push_token(&tokens, SEP , " ");
//     // push_token(&tokens, WORD, "-la");
//     // push_token(&tokens, SEP , " ");
//     // push_token(&tokens, WORD, "/usr/bin");


//     // push_token(&tokens, WORD, "grep");        
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, WORD, "-i");          
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, WORD, "pattern");     
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, REDIRECT_IN,  "<");   
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, WORD, "in.txt");      
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, REDIRECT_OUT, ">");   
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, WORD, "out.txt");

//     // push_token(&tokens, WORD, "grep");         
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, WORD, "-i");           
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, WORD, "pattern");      
//     // push_token(&tokens, SEP, " ");
//     // push_token(&tokens, REDIRECT_IN,  "<");    

//     // push_token(&tokens, HEREDOC,  "<");    push_token(&tokens, SEP, " ");
//     // push_token(&tokens, REDIRECT_OUT,  "<");    push_token(&tokens, SEP, " ");
//     // push_token(&tokens, REDIRECT_APPEND,  "<");    push_token(&tokens, SEP, " ");


//     // push_token(&tokens, WORD, "ls");
//     // push_token(&tokens, SEP , " ");
//     // push_token(&tokens, WORD, "-la");
//     // push_token(&tokens, FIELD, "-la");
//     // push_token(&tokens, EXP_FIELD, "-la");


//     // push_token(&tokens, WORD, "ls");
//     // push_token(&tokens, SEP , " ");
//     // push_token(&tokens, WORD, "-la");
//     // push_token(&tokens, FIELD, "$USER");
//     // push_token(&tokens, EXP_FIELD, "$USER");
//     // push_token(&tokens, EXP_FIELD, "$PWD");


//     // push_token(&tokens, WORD, "ls");
//     // push_token(&tokens, SEP , " ");
//     // push_token(&tokens, WORD, "-la");
//     // push_token(&tokens, FIELD, "-la");
//     // push_token(&tokens, REDIRECT_OUT, ">");
//     // push_token(&tokens, WORD, "-la");
#include "../header/header.h"

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
            if (need > cap) { 
                cap = need * 2; 
                char *new_out = realloc(out, cap); 
                if (!new_out) { 
                    free(out); 
                    return NULL; 
                }
                out = new_out;
            }
            strcpy(out + len, val);
            len += strlen(val);
        }
        else
        {
            if (len + 2 > cap) { 
                cap *= 2; 
                char *new_out = realloc(out, cap); 
                if (!new_out) { 
                    free(out); 
                    return NULL; 
                }
                out = new_out;
            }
            out[len++] = *p++;
            out[len]   = '\0';
        }
    }
    return out;
}

/* forward declaration */
void free_groups(t_list **groups);

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
    
    t_list *node = ft_lstnew(cmd);
    if (!node) {
        free(cmd->arg);
        free(cmd);
        return 0;
    }
    
    ft_lstadd_back(&grp->argv, node);
    return 1;
}

/* ──────────────────── parser 1-D → 2-D ────────────────────────────────── */

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
                
                t_list *group_node = ft_lstnew(cur);
                if (!group_node) {
                    free(cur);
                    free_groups(&groups);
                    return NULL;
                }
                ft_lstadd_back(&groups, group_node);
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
                
                t_list *group_node = ft_lstnew(cur);
                if (!group_node) {
                    free(cur);
                    free_groups(&groups);
                    return NULL;
                }
                ft_lstadd_back(&groups, group_node);
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
                if (!piece) { 
                    free_groups(&groups); 
                    free(arg); 
                    return NULL; 
                }

                size_t plen = strlen(piece);
                char *tmp   = realloc(arg, len + plen + 1);
                if (!tmp)   { 
                    free_groups(&groups); 
                    free(arg); 
                    free(piece); 
                    return NULL; 
                }
                arg = tmp;
                memcpy(arg + len, piece, plen);
                len += plen;
                arg[len] = '\0';
                free(piece);
                scan = scan->next;
            }

            if (!add_argument(cur, arg))
            {   
                free_groups(&groups); 
                free(arg); 
                return NULL; 
            }
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

t_list *parser(t_list *tokens)
{
  if (!tokens)
    return NULL;

  t_list *groups = tokens_to_groups(tokens);
  if (!groups)
    return NULL;

  return groups;
}
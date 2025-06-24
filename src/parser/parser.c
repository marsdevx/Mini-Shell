/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/24 20:15:15 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	is_redirect(e_type t)
{
	return (t == REDIRECT_IN || t == REDIRECT_OUT || t == REDIRECT_APPEND
		|| t == HEREDOC);
}

static int	is_text(e_type t)
{
	return (t == WORD || t == FIELD || t == EXP_FIELD);
}

t_command	*new_command(const char *arg)
{
	t_command	*c;

	c = malloc(sizeof *c);
	if (!c)
		return (NULL);
	c->arg = strdup(arg);
	if (!c->arg)
	{
		free(c);
		return (NULL);
	}
	return (c);
}

t_group	*new_group(void)
{
	t_group	*g;

	g = malloc(sizeof *g);
	if (!g)
		return (NULL);
	g->argv = NULL;
	return (g);
}

int	add_argument(t_group *grp, const char *arg)
{
	t_command	*cmd;
	t_list		*node;

	cmd = new_command(arg);
	if (!cmd)
		return (0);
	node = ft_lstnew(cmd);
	if (!node)
	{
		free(cmd->arg);
		free(cmd);
		return (0);
	}
	ft_lstadd_back(&grp->argv, node);
	return (1);
}

void	free_groups(t_list **groups)
{
	t_list	*nextg;
	t_group	*grp;
	t_list	*nexta;

	while (*groups)
	{
		nextg = (*groups)->next;
		grp = (*groups)->content;
		while (grp->argv)
		{
			nexta = grp->argv->next;
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



static char *expand_word_env(const char *src)
{
    const char *p = src;
    size_t  cap = strlen(src) + 256;
    size_t  len = 0;
    char   *out = malloc(cap);
    if (!out) return NULL;

    while (*p)
    {
        if (*p == '$' && (isalpha((unsigned char)p[1]) || p[1] == '_' || p[1] == '?'))
        {
            const char *v = ++p;
            
            if (*p == '?')
            {
                p++;
                const char *val = getenv("?");
                if (!val) val = "0";
                
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
        }
        else if (*p == '$' && !isalpha((unsigned char)p[1]) && p[1] != '_' && p[1] != '?')
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

t_list *tokens_to_groups(t_list *tok_lst)
{
    t_list  *groups = NULL;
    t_group *cur    = NULL;

    while (tok_lst)
    {
        t_token *tk = tok_lst->content;

        if (is_redirect(tk->type))
        {
            t_list *look = tok_lst->next;
            if (look && ((t_token *)look->content)->type == SEP)
                look = look->next;
            if (!look || !is_text(((t_token *)look->content)->type))
            {
                fprintf(stderr,
                        "syntax error: redirect \"%s\" needs a filename\n",
                        tk->value);
                free_groups(&groups);
                return NULL;
            }

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

            if (!add_argument(cur, tk->value))
            {   free_groups(&groups); return NULL; }

            tok_lst = tok_lst->next;
            continue;
        }

        if (tk->type == PIPE) {
            cur = NULL; 
            tok_lst = tok_lst->next; 
            continue;
        }

        if (tk->type == SEP) { 
            tok_lst = tok_lst->next; 
            continue; 
        }

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
                char    *piece;
                
                if (tk2->type == EXP_FIELD || tk2->type == WORD)
                    piece = expand_word_env(tk2->value);
                else
                    piece = strdup(tk2->value);
                    
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

            if (strlen(arg) > 0 || ((t_group *)groups->content)->argv == NULL)
            {
                if (!add_argument(cur, arg))
                {   
                    free_groups(&groups); 
                    free(arg); 
                    return NULL; 
                }
            }
            free(arg);
            tok_lst = scan;
            continue;
        }

        tok_lst = tok_lst->next;
    }
    return groups;
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
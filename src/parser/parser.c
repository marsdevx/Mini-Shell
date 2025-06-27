/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/27 18:21:07 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static int	is_redirect(t_e_type t)
{
	return (t == REDIRECT_IN || t == REDIRECT_OUT || t == REDIRECT_APPEND
		|| t == HEREDOC);
}

static int	is_text(t_e_type t)
{
	return (t == WORD || t == FIELD || t == EXP_FIELD);
}

static int	is_valid_var_char(char c, int first)
{
	if (first)
		return (ft_isalpha(c) || c == '_');
	return (ft_isalnum(c) || c == '_');
}

t_command	*new_command(const char *arg)
{
	t_command	*c;

	c = malloc(sizeof(*c));
	if (!c)
		return (NULL);
	c->arg = ft_strdup(arg);
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

	g = malloc(sizeof(*g));
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

static char	*expand_env_var(const char *var_name, char **env)
{
	const char	*val;

	if (ft_strncmp(var_name, "?", 2) == 0)
	{
		val = get_env_value(env, "?");
		if (!val)
			val = "0";
	}
	else
	{
		val = get_env_value(env, var_name);
		if (!val)
			val = "";
	}
	return (ft_strdup(val));
}

static char	*get_var_name(const char *str, int *len)
{
	char	*var_name;
	int		i;

	if (*str == '?')
	{
		*len = 1;
		return (ft_strdup("?"));
	}
	i = 0;
	while (str[i] && is_valid_var_char(str[i], i == 0))
		i++;
	*len = i;
	if (i == 0)
		return (NULL);
	var_name = malloc(i + 1);
	if (!var_name)
		return (NULL);
	ft_strlcpy(var_name, str, i + 1);
	return (var_name);
}

static char	*join_strings(char *s1, char *s2)
{
	char	*result;
	int		len1;
	int		len2;
	int		i;
	int		j;

	if (!s1 && !s2)
		return (NULL);
	len1 = s1 ? ft_strlen(s1) : 0;
	len2 = s2 ? ft_strlen(s2) : 0;
	result = malloc(len1 + len2 + 1);
	if (!result)
		return (NULL);
	i = 0;
	while (s1 && i < len1)
	{
		result[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2 && j < len2)
	{
		result[i + j] = s2[j];
		j++;
	}
	result[i + j] = '\0';
	return (result);
}

static char	*expand_word_env(const char *src, char **env)
{
	const char	*p;
	char		*result;
	char		*temp;
	char		*var_name;
	char		*var_value;
	char		*new_result;
	int			var_len;

	p = src;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (*p)
	{
		if (*p == '$' && (is_valid_var_char(p[1], 1) || p[1] == '?'))
		{
			p++;
			var_name = get_var_name(p, &var_len);
			if (!var_name)
			{
				free(result);
				return (NULL);
			}
			var_value = expand_env_var(var_name, env);
			free(var_name);
			if (!var_value)
			{
				free(result);
				return (NULL);
			}
			new_result = join_strings(result, var_value);
			free(result);
			free(var_value);
			if (!new_result)
				return (NULL);
			result = new_result;
			p += var_len;
		}
		else if (*p == '$' && !is_valid_var_char(p[1], 1) && p[1] != '?')
		{
			temp = malloc(2);
			if (!temp)
			{
				free(result);
				return (NULL);
			}
			temp[0] = *p;
			temp[1] = '\0';
			new_result = join_strings(result, temp);
			free(result);
			free(temp);
			if (!new_result)
				return (NULL);
			result = new_result;
			p++;
		}
		else
		{
			temp = malloc(2);
			if (!temp)
			{
				free(result);
				return (NULL);
			}
			temp[0] = *p;
			temp[1] = '\0';
			new_result = join_strings(result, temp);
			free(result);
			free(temp);
			if (!new_result)
				return (NULL);
			result = new_result;
			p++;
		}
	}
	return (result);
}

static int	handle_redirect(t_token *tk, t_list *tok_lst, t_group **cur, t_list **groups)
{
	t_list	*look;
	t_list  *group_node;

	look = tok_lst->next;
	if (look && ((t_token *)look->content)->type == SEP)
		look = look->next;
	if (!look || !is_text(((t_token *)look->content)->type))
	{
		write(2, "syntax error: redirect \"", 24);
		write(2, tk->value, ft_strlen(tk->value));
		write(2, "\" needs a filename\n", 19);
		return (0);
	}
	if (!*cur)
	{
		*cur = new_group();
		if (!*cur)
			return (0);
		group_node = ft_lstnew(*cur);
		if (!group_node)
		{
			free(*cur);
			return (0);
		}
		ft_lstadd_back(groups, group_node);
	}
	return (add_argument(*cur, tk->value));
}

static char	*concatenate_text_tokens(t_list **scan, char **env)
{
	char	*arg;
	char	*piece;
	char	*new_arg;
	t_token	*tk2;

	arg = ft_strdup("");
	if (!arg)
		return (NULL);
	while (*scan && is_text(((t_token *)(*scan)->content)->type))
	{
		tk2 = (*scan)->content;
		if (tk2->type == EXP_FIELD || tk2->type == WORD)
			piece = expand_word_env(tk2->value, env);
		else
			piece = ft_strdup(tk2->value);
		if (!piece)
		{
			free(arg);
			return (NULL);
		}
		new_arg = join_strings(arg, piece);
		free(arg);
		free(piece);
		if (!new_arg)
			return (NULL);
		arg = new_arg;
		*scan = (*scan)->next;
	}
	return (arg);
}

static int	handle_text(t_list **tok_lst, t_group **cur, t_list **groups, char **env)
{
	char	*arg;
	t_list	*scan;
	t_list  *group_node;

	if (!*cur)
	{
		*cur = new_group();
		if (!*cur)
			return (0);
		group_node = ft_lstnew(*cur);
		if (!group_node)
		{
			free(*cur);
			return (0);
		}
		ft_lstadd_back(groups, group_node);
	}
	scan = *tok_lst;
	arg = concatenate_text_tokens(&scan, env);
	if (!arg)
		return (0);
	
	// FIXED: Only add argument if it's not empty OR if it's the only token being processed
	// This handles the case where $EMPTY expands to empty string and should be ignored
	if (ft_strlen(arg) > 0)
	{
		if (!add_argument(*cur, arg))
		{
			free(arg);
			return (0);
		}
	}
	// If argument is empty and we have no arguments yet, we might still want to add it
	// in some contexts, but for variable expansion to empty, we skip it
	
	free(arg);
	*tok_lst = scan;
	return (1);
}

t_list	*tokens_to_groups(t_list *tok_lst, char **env)
{
	t_list	*groups;
	t_group	*cur;
	t_token	*tk;

	groups = NULL;
	cur = NULL;
	while (tok_lst)
	{
		tk = tok_lst->content;
		if (is_redirect(tk->type))
		{
			if (!handle_redirect(tk, tok_lst, &cur, &groups))
			{
				free_groups(&groups);
				return (NULL);
			}
			tok_lst = tok_lst->next;
		}
		else if (tk->type == PIPE)
		{
			cur = NULL;
			tok_lst = tok_lst->next;
		}
		else if (tk->type == SEP)
		{
			tok_lst = tok_lst->next;
		}
		else if (is_text(tk->type))
		{
			if (!handle_text(&tok_lst, &cur, &groups, env))
			{
				free_groups(&groups);
				return (NULL);
			}
		}
		else
			tok_lst = tok_lst->next;
	}
	return (groups);
}

t_list	*parser(t_list *tokens, char **env)
{
	t_list	*groups;

	if (!tokens)
		return (NULL);
	groups = tokens_to_groups(tokens, env);
	if (!groups)
		return (NULL);
	return (groups);
}
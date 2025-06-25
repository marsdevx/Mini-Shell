/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:10 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 23:36:00 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

int ft_setenv(const char *name, const char *value, int overwrite)
{
    extern char **environ;
    size_t    len;
    char     *pair;
    int       i;
    char    **new_env;

    if (!name || !*name || ft_strchr(name, '='))
        return (-1);
    if (!value)
        value = "";

    len = ft_strlen(name);

    for (i = 0; environ[i]; i++)
    {
        if (ft_strncmp(environ[i], name, len) == 0
            && environ[i][len] == '=')
        {
            if (!overwrite)
                return (0);

            pair = ft_strjoin(name, "=");
            if (!pair)
                return (-1);
            {
                char *tmp = ft_strjoin(pair, value);
                free(pair);
                if (!tmp)
                    return (-1);
                free(environ[i]);
                environ[i] = tmp;
            }
            return (0);
        }
    }

    pair = ft_strjoin(name, "=");
    if (!pair)
        return (-1);
    {
        char *tmp = ft_strjoin(pair, value);
        free(pair);
        if (!tmp)
            return (-1);
        pair = tmp;
    }

    new_env = malloc(sizeof(char *) * (i + 2));
    if (!new_env)
    {
        free(pair);
        return (-1);
    }
		
    for (int j = 0; j < i; j++)
        new_env[j] = environ[j];
    new_env[i]     = pair;
    new_env[i + 1] = NULL;

    environ = new_env;
    return (0);
}

t_list	*ft_lstnew(void *content)
{
	t_list	*node;

	node = malloc(sizeof(t_list));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (!lst || !new)
		return ;
	if (!*lst)
		*lst = new;
	else
	{
		last = *lst;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}

void	ft_free_tokens(t_list **tokens)
{
	t_list	*current;
	t_list	*next;
	t_token	*token;

	current = *tokens;
	while (current)
	{
		next = current->next;
		token = (t_token *)current->content;
		free(token->value);
		free(token);
		free(current);
		current = next;
	}
	*tokens = NULL;
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;

	dup = malloc(n + 1);
	if (!dup)
		return (NULL);
	strncpy(dup, s, n);
	dup[n] = '\0';
	return (dup);
}

char *ft_strcpy(char *dest, const char *src) {
  char *d = dest;
  while (*src) {
    *d++ = *src++;
  }
  *d = '\0';
  return dest;
}

char *ft_strncpy(char *dest, const char *src, size_t n) {
  size_t i = 0;
  while (i < n && src[i]) {
    dest[i] = src[i];
    i++;
  }
  while (i < n) {
    dest[i++] = '\0';
  }
  return dest;
}

int ft_strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}

char *ft_strcat(char *dest, const char *src) {
  char *d = dest;
  while (*d) {
    d++;
  }
  while (*src) {
    *d++ = *src++;
  }
  *d = '\0';
  return dest;
}

char *ft_strtok(char *str, const char *delim) {
  static char *next;
  char *start;
  char *p;

  if (str)
    next = str;
  if (!next)
    return NULL;

  p = next;
  while (*p) {
    const char *d = delim;
    int is_delim = 0;
    while (*d) {
      if (*p == *d) {
        is_delim = 1;
        break;
      }
      d++;
    }
    if (!is_delim)
      break;
    p++;
  }
  if (!*p) {
    next = NULL;
    return NULL;
  }
  start = p;

  while (*p) {
    const char *d = delim;
    int is_delim = 0;
    while (*d) {
      if (*p == *d) {
        is_delim = 1;
        break;
      }
      d++;
    }
    if (is_delim) {
      *p = '\0';
      next = p + 1;
      return start;
    }
    p++;
  }
  next = NULL;
  return start;
}

long ft_strtol(const char *str, char **endptr, int base) {
  const char *s = str;
  int sign = 1;
  long result = 0;

  while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\v' || *s == '\f' || *s == '\r')
    s++;

  if (*s == '+' || *s == '-') {
    if (*s == '-')
      sign = -1;
    s++;
  }

  if (base == 0) {
    if (*s == '0') {
      if (s[1] == 'x' || s[1] == 'X') {
        base = 16;
        s += 2;
      } else {
        base = 8;
        s++;
      }
    } else {
      base = 10;
    }
  } else if (base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
    s += 2;
  }

  const char *start = s;
  while ((*s >= '0' && *s <= '9') || (*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z')) {
    int digit;
    if (*s >= '0' && *s <= '9')
      digit = *s - '0';
    else if (*s >= 'A' && *s <= 'Z')
      digit = *s - 'A' + 10;
    else
      digit = *s - 'a' + 10;
    if (digit >= base)
      break;
    result = result * base + digit;
    s++;
  }

  if (endptr)
    *endptr = (char *)(start == s ? str : s);

  return sign * result;
}

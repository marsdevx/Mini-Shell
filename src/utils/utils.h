#ifndef UTILS_H
# define UTILS_H

typedef struct s_list
{
  void          *content;
  struct s_list *next;
} t_list;

t_list *ft_lstnew(void *content);
void ft_lstadd_back(t_list **lst, t_list *new);
void ft_free_tokens(t_list **tokens);

#endif
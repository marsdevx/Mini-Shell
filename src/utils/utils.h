#ifndef UTILS_H
# define UTILS_H

typedef struct s_list
{
  void          *content;
  struct s_list *next;
} t_list;

t_list *ft_lstnew(void *content);

#endif
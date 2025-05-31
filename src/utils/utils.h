#ifndef UTILS_H
# define UTILS_H

# include "../init/init.h"


t_list *ft_lstnew(void *content);
void ft_lstadd_back(t_list **lst, t_list *new);
void ft_free_tokens(t_list **tokens);
void ft_print_tokens(t_list *tokens);
char *ft_strndup(const char *s, size_t n);


#endif
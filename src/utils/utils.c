#include "../header/header.h"

/* Helper function: Create a new linked list node */
t_list *ft_lstnew(void *content)
{
    t_list *node = malloc(sizeof(t_list));
    if (!node)
        return NULL;
    node->content = content;
    node->next = NULL;
    return node;
}

/* Helper function: Add a node to the back of the list */
void ft_lstadd_back(t_list **lst, t_list *new)
{
    if (!lst || !new)
        return;
    if (!*lst)
        *lst = new;
    else
    {
        t_list *last = *lst;
        while (last->next)
            last = last->next;
        last->next = new;
    }
}
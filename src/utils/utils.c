#include "../init/header.h"

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

/* Free the token list and its contents */
void ft_free_tokens(t_list **tokens)
{
    t_list *current = *tokens;
    while (current)
    {
        t_list *next = current->next;
        t_token *token = (t_token *)current->content;
        free(token->value);
        free(token);
        free(current);
        current = next;
    }
    *tokens = NULL;
}

/* Print the tokens for debugging */ 
//need only for debug
void ft_print_tokens(t_list *tokens)
{
    t_list *current = tokens;
    while (current)
    {
        t_token *token = (t_token *)current->content;
        printf("Type: %d, Value: %s, Length: %d\n", token->type, token->value, token->value_len);
        current = current->next;
    }
}

char *ft_strndup(const char *s, size_t n)
{
    char *dup = malloc(n + 1);
    if (!dup)
        return NULL;
    strncpy(dup, s, n);
    dup[n] = '\0';
    return dup;
}
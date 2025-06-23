#ifndef PARSER_H
# define PARSER_H

#include "../init/init.h"

t_list *parser(t_list *tokens);
void free_groups(t_list **groups);

#endif
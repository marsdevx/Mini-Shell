#ifndef LEXER_H
# define LEXER_H

#include "../init/init.h"

int quotes_check(char *input);
static void process_input(char *line, t_info *info);
t_list *lexer(char *input);


#endif
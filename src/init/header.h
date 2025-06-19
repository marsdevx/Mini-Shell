#ifndef HEADER_H
# define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Our header files
#include "../utils/utils.h"
#include "../lexer/lexer.h"
#include "../reader/reader.h"
#include "../parser/parser.h"
#include "../builtin/builtin.h"
#include "../exec/exec.h"

#endif
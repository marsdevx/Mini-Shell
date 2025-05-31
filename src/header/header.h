#ifndef HEADER_H
# define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

// Our header files
// #include "../utils/utils.h"

/* Enum for token types */
typedef enum e_type
{
    WORD,           // Regular words
    FIELD,          // Quoted fields (e.g., 'text')
    EXP_FIELD,      // Expandable fields (e.g., "text")
    SEP,            // Separators (spaces/tabs)
    PIPE,           // Pipe symbol (|)
    REDIRECT_IN,    // Input redirection (<)
    REDIRECT_OUT,   // Output redirection (>)
    REDIRECT_APPEND,// Append redirection (>>)
    HEREDOC         // Heredoc redirection (<<)
} e_type;

typedef struct s_list
{
    void          *content;
    struct s_list *next;
} t_list;


/* Structure for a token */
typedef struct s_token
{
    e_type type;      // Token type
    char *value;      // Token value as a string
    int   value_len;  // Length of the value
} t_token;


typedef struct s_command { char *arg; } t_command;
typedef struct s_group   { t_list *argv; } t_group;

#endif
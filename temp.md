# Notes
read line - only read and save 
lexer - only error not closed quotes

# First steps
basic file structure
git
what is input for each func, what is output from each func?
how to make work all of commands from shell inside our program

# Whole program
1) init (built-in, vars)
2) read
3) lexer (tokens)
4) parser (groups, expansions, collect info)
5) exec (redirect, pipe)
6) destr (meamory leaks)


выписать все возможноые примеры что может приходить в лексер и парсер и какие корректные варианты обработки

typedef token {
  int len;
  char *word;
  char *type;
} t_token;

typedef linkedlist
{
  t_token* token;
  t_token** next;
}
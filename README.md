# Version 1
./minishell
minishell > hello
minishell > hello

## V1 --work
readline(PROMPT) -> save str after minishell> in var, and write minishell> while program started
ft_strncmp(input, EXIT_CMD, sizeof(EXIT_CMD)) == 0 -> compare input with existing commands, if cont print error


# Version 2
echo, echo -n, cd, pwd, exprot, unset, env, exit (without any characters or shortcuts)


# Version 3

	1.	Unclosed Quotes:
	•	Detect unclosed single (') or double quotes (") and prevent the command from executing, displaying an error message.
	2.	Single Quotes ('):
	•	Treat everything inside single quotes literally.
	•	Special characters like $, \, *, etc., should not be interpreted.
	3.	Double Quotes ("):
	•	Treat everything inside double quotes literally except for $.
	•	Variables (e.g., $USER) should be expanded.
	4.	Ignore Special Characters:
	•	Do not interpret special characters like \ (backslash) or ; (semicolon).


## cd command 
Single Quotes ('): The path inside single quotes should be taken literally, without expanding any special characters or variables.
Double Quotes ("): The path inside double quotes allows variable expansion.
Unclosed Quotes: If quotes are unclosed, cd should return an error and not execute.

## export command
Single Quotes ('): Prevents variable expansion in the value.
Double Quotes ("): Allows variable expansion in the value.
Unclosed Quotes: Should return an error and not set the variable.

## unset command 
Single or Double Quotes: Quotes can be used around the variable name, and they should be handled correctly.
unset 'VAR'
unset "VAR"
These should both work and unset VAR.

Unclosed Quotes: Should return an error and not execute.
unset 'VAR
unset "VAR
syntax error: unclosed quotes

Simple usage
unset VAR



## echo command
Unclosed Quotes: If quotes are unclosed, cd should return an error and not execute.
echo "Hello, World
error: unclosed quotes

Single Quotes: variable must not be expanded
echo 'Hello $USER'
echo '$RANDOM'
$RANDOM
Hello $USER

Double Quotes: variable must be expanded
echo $USER
mark


echo $RANDOM

echo "Hello $USER"
Hello mark


Ignoring Special Characters: (\, ;) (not ignoring $)
	echo Hello\World
	Hello\World
	echo Hello; World
	Hello; World








# Allowed funcs:
readline, rl_clear_history, rl_on_new_line,
rl_replace_line, rl_redisplay, add_history,
printf, malloc, free, write, access, open, read,
close, fork, wait, waitpid, wait3, wait4, signal,
sigaction, sigemptyset, sigaddset, kill, exit,
getcwd, chdir, stat, lstat, fstat, unlink, execve,
dup, dup2, pipe, opendir, readdir, closedir,
strerror, perror, isatty, ttyname, ttyslot, ioctl,
getenv, tcsetattr, tcgetattr, tgetent, tgetflag,
tgetnum, tgetstr, tgoto, tputs
ft_calloc.c     ft_isascii.c    ft_itoa.c       ft_memcpy.c     ft_putchar_fd.c ft_putstr_fd.c  ft_strdup.c     ft_strlcat.c    ft_strmapi.c    ft_strrchr.c    ft_tolower.c
ft_atoi.c       ft_isalnum.c    ft_isdigit.c    ft_memchr.c     ft_memmove.c    ft_putendl_fd.c ft_split.c      ft_striteri.c   ft_strlcpy.c    ft_strncmp.c    ft_strtrim.c    ft_toupper.c
ft_bzero.c      ft_isalpha.c    ft_isprint.c    ft_memcmp.c     ft_memset.c     ft_putnbr_fd.c  ft_strchr.c     ft_strjoin.c    ft_strlen.c     ft_strnstr.c    ft_substr.c



# Change for linux
1)  clear_history, on_new_line, - rl_clear_history, rl_on_new_line,
2) makefile - 
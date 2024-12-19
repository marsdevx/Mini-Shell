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




## export command
Single Quotes ('): Prevents variable expansion in the value.
Double Quotes ("): Allows variable expansion in the value.
Unclosed Quotes: Should return an error and not set the variable.




# All funcs explanathion
--------------------------------------------------------------------------
## Echo
minishell> echo home (same: echo "home") (same: echo 'home')
minishell> home

minishell> echo -n home (same: echo -n "home") (same: echo -n 'home')
homeminishell> 

minishell> echo $HOME    (same: echo "$HOME")
/Users/marksylaiev
minishell>

minishell> echo $NONEXIST    (same: echo "$NONEXIST")

minishell>

minishell> echo '$HOME'
$HOME
minishell>

minishell> echo "home    (same: echo 'home)
error: unclosed quotes
minishell>

minishell> echo Hello\World
Hello\World
minishell> 

minishell> echo Hello;World
Hello;World
minishell> 
-----------------
## Full program example:
./minishell
minishell> echo home
home
minishell> echo "home"
home
minishell> echo 'home'
home
minishell> echo -n home
homeminishell> echo -n "home"
homeminishell> echo -n 'home'
homeminishell> echo $HOME
/Users/marksylaiev
minishell> echo "$HOME"
/Users/marksylaiev
minishell> echo $NONEXIST

minishell> echo "$NONEXIST"

minishell> echo '$HOME'
$HOME
minishell> echo 'home
minishell: error: unclosed quotes
minishell> echo "home
minishell: error: unclosed quotes
minishell> echo Hello\World
Hello\World
minishell> echo Hello;World
Hello;World
minishell> 
--------------------------------------------------------------------------
## Unset
minishell> echo $HOME
/Users/marksylaiev

minishell> unset VAR  (same: unset 'VAR') (same: unset "VAR")
minishell> echo $HOME

minishell>

minishell> unset NONEXIST
minishell: error: var not exist

minishell> unset
minishell: error: var not exist

minishell> unset 'VAR    (same: unset "VAR)
minishell: error: unclosed quotes
-----------------
## Full program example:
❯ ./minishell
minishell> echo $HOME
/Users/marksylaiev
minishell> unset HOME
minishell> echo $HOME

minishell> exit
exit
❯ ./minishell
minishell> echo $HOME
/Users/marksylaiev
minishell> unset "HOME"
minishell> echo $HOME

minishell> exit
exit
❯ ./minishell
minishell> echo $HOME
/Users/marksylaiev
minishell> unset 'HOME'
minishell> echo $HOME

minishell> unset NONEXIST
minishell: error: var not exist
minishell> unset
minishell: error: var not exist
minishell> unset 'HOME
minishell: error: unclosed quotes
minishell> unset "HOME
minishell: error: unclosed quotes
--------------------------------------------------------------------------
## Cd
minishell> pwd
/Users/marksylaiev/Desktop/Projects/42-school/42-core/My-Shell

minishell> cd ..
minishell> pwd
/Users/marksylaiev/Desktop/Projects/42-school/42-core

minishell> cd ../../..
minishell> pwd
/Users/marksylaiev/Desktop

minishell> cd ~/Desktop/Projects/42-school/42-core/My-Shell/
minishell> pwd
/Users/marksylaiev/Desktop/Projects/42-school/42-core/My-Shell

minishell> cd '~/../../../Users/$USER'
minishell: cd: No such file or directory

minishell> cd ~/../../../Users/nonexist
minishell: cd: No such file or directory

minishell> cd "~/../../../Users/$USER"  (same: cd ~/../../../Users/$USER)
minishell> pwd
/Users/marksylaiev

minishell> cd "~/../../../Users/marksylaiev    (same: cd '~/../../../Users/marksylaiev)
minishell: error: unclosed quotes
-----------------
## Full program example:
❯ ./minishell
minishell> pwd
/Users/marksylaiev/Desktop/Projects/42-school/42-core/My-Shell
minishell> cd ..
minishell> pwd
/Users/marksylaiev/Desktop/Projects/42-school/42-core
minishell> cd ../../..
minishell> pwd
/Users/marksylaiev/Desktop
minishell> cd ~/Desktop/Projects/42-school/42-core/My-Shell/
minishell> pwd
/Users/marksylaiev/Desktop/Projects/42-school/42-core/My-Shell
minishell> cd '~/../../../Users/$USER'
minishell: cd: /Users/marksylaiev/../../../Users/$USER: No such file or directory
minishell> cd ~/../../../Users/nonexist
minishell: cd: /Users/marksylaiev/../../../Users/nonexist: No such file or directory
minishell> cd "~/../../../Users/$USER"
minishell> pwd
/Users/marksylaiev
minishell> cd ~/../../../Users/$USER
minishell> pwd
/Users/marksylaiev
minishell> cd "~/../../../Users/marksylaiev
minishell: error: unclosed quotes
minishell> cd '~/../../../Users/marksylaiev
minishell: error: unclosed quotes
--------------------------------------------------------------------------













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
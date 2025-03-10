# Symbols 

## $VAR
  $VARNAME, "$VARNAME" - var
  '$VARNAME' - string

## (>) (>>)
  command > file.txt  # Overwrites file.txt with output from command
  command >> file.txt  # Append file.txt with output from command




# Builtins
--------------------------------------------------------------------------
1. Echo
minishell> echo home (same: echo "home") (same: echo 'home')
minishell> home

minishell> echo hello world (same: echo "hello world") (same: echo 'hello world')
minishell> hello world

minishell> echo -n home (same: echo -n "home") (same: echo -n 'home')
homeminishell> 

minishell> echo -n -n -n home (same: echo -n -n -n "home") (same: echo -n -n -n 'home')
homeminishell> 

minishell> echo -n -v -n home (same: echo -n -v -n "home") (same: echo -n -v -n 'home')
-v -n homeminishell> 

minishell> echo -n hello world (same: echo -n "hello world") (same: echo -n 'hello world')
hello worldminishell> 

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
### Full program example:
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
2. Unset
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
### Full program example:
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
3. Cd
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
### Full program example:
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
4. Export
minishell> export
(Lists all environment variables in alphabetical order)

minishell> export VAR="Hello World"    (same: export VAR='Hello World')
minishell> echo $VAR
Hello World
minishell>

minishell> export VAR="Hello $USER"
minishell> echo $VAR
Hello marksylaiev
minishell> 

minishell> export VAR='Hello $USER'
minishell> echo $VAR
Hello $USER
minishell>

minishell> export VAR="Hello World    (same: export VAR='Hello World)
minishell: error: unclosed quotes
-----------------
### Full program example:
❯ ./minishell
minishell> export VAR="Hello World"
minishell> echo $VAR
Hello World
minishell> export VAR='Hello World'
minishell> echo $VAR
Hello World
minishell> export VAR="Hello $USER"
minishell> echo $VAR
Hello marksylaiev
minishell> export VAR='Hello $USER'
minishell> echo $VAR
Hello $USER
minishell> export VAR="Hello World
minishell: export: error: unclosed quotes
minishell> export VAR='Hello World
minishell: export: error: unclosed quotes
minishell> export
declare -x COLORFGBG=15;0
declare -x COLORTERM=truecolor
declare -x COMMAND_MODE=unix2003
declare -x GIT_ASKPASS=/Applications/Visual Studio Code.app/Contents/Resources/app/extensions/git/dist/askpass.sh
declare -x HOME=/Users/marksylaiev
--------------------------------------------------------------------------
5. Pwd
❯ ./minishell
minishell> pwd
/Users/marksylaiev/Desktop/Projects/42-school/42-core/My-Shell
--------------------------------------------------------------------------
6. Env
❯ ./minishell
minishell> env
PWD=/Users/marksylaiev/Desktop/Projects/42-school/42-core/My-Shell
ITERM_PROFILE=Default
USER=marksylaiev
--------------------------------------------------------------------------
7. Exit
❯ ./minishell
minishell> exit
exit
--------------------------------------------------------------------------


# Redirections
--------------------------------------------------------------------------
1. Input Redirection: <

minishell>  echo "Hello World!" > input.txt
minishell>  echo < input.txt
minishell>  Hello World!

echo "../" > path.txt
cd < path.txt
pwd
--------------------------------------------------------------------------
2. Output Redirection: >
echo -n "Hello, World!" > output.txt
pwd > current_dir.txt
env > environment.txt
--------------------------------------------------------------------------
3. Here-Document: <<
echo << EOF
Hello, this is a here-document.
This is another line.
EOF

pwd << STOP
STOP
--------------------------------------------------------------------------
4. Append Output Redirection: >>
echo -n "First line." >> append.txt
echo -n " Second line." >> append.txt

pwd >> append.txt

env >> environment_log.txt
--------------------------------------------------------------------------
5. Combine










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
1)  clear_history, on_new_line, === rl_clear_history, rl_on_new_line,
2) makefile - 
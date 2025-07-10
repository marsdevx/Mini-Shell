# Vars
CC = cc
CFLAGS = -I$(SRCDIR) -I$(LIBFTDIR) -I/opt/homebrew/opt/readline/include  -Wall -Wextra -Werror 
NAME = minishell
SRCDIR = src
OBJDIR = obj
LIBFTDIR = libs/libft

SRC_BUILTIN = builtin/builtin.c builtin/utils.c builtin/cd.c builtin/export.c
SRC_ENV = env/env.c env/utils.c env/init.c
SRC_EXEC = exec/exec.c exec/external.c exec/path.c exec/redir.c exec/redir_utils.c exec/utils.c exec/heredoc.c exec/exec_utils.c
SRC_INIT = init/main.c init/reader.c init/init_shell_env.c
SRC_LEXER = lexer/lexer.c lexer/handle.c lexer/handle_utils.c lexer/utils.c
SRC_PARSER = parser/parser.c parser/utils.c parser/expand.c parser/expand_utils.c parser/handlers.c parser/checker.c parser/groups.c
SRC_PIPE = pipe/pipe.c pipe/child.c pipe/exec.c pipe/external.c pipe/setup.c
SRC_UTILS = utils/utils.c utils/str.c
SRC = $(SRC_BUILTIN) $(SRC_ENV) $(SRC_EXEC) $(SRC_INIT) $(SRC_LEXER) $(SRC_PARSER) $(SRC_PIPE) $(SRC_UTILS)

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))
SRCS = $(addprefix $(SRCDIR)/, $(SRC))

# Rules of files
$(NAME): $(OBJS) $(LIBFTDIR)/libft.a
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFTDIR) -lft -L/opt/homebrew/opt/readline/lib -lreadline -o $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)/builtin $(OBJDIR)/env $(OBJDIR)/exec $(OBJDIR)/init $(OBJDIR)/lexer $(OBJDIR)/parser $(OBJDIR)/pipe $(OBJDIR)/utils

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFTDIR)/libft.a:
	$(MAKE) -C $(LIBFTDIR)

# Rules of actions
.PHONY: all clean fclean re

all: $(NAME)

clean:
	rm -rf $(OBJDIR)
	$(MAKE) -C $(LIBFTDIR) clean

fclean: clean
	rm -rf $(NAME)
	$(MAKE) -C $(LIBFTDIR) fclean

re: fclean all
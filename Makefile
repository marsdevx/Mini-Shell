# Vars
CC = cc
CFLAGS = -I$(SRCDIR) -I$(LIBFTDIR) -I/opt/homebrew/opt/readline/include  -Wall -Wextra -Werror 
NAME = minishell
SRCDIR = src
OBJDIR = obj
LIBFTDIR = libs/libft

SRC_BUILTIN = builtin/builtin.c
SRC_EXEC = exec/exec.c exec/external.c exec/path.c exec/redirections.c exec/pipe.c exec/env_utils.c
SRC_INIT = init/main.c
SRC_LEXER = lexer/lexer.c 
SRC_PARSER = parser/parser.c parser/utils.c parser/expand.c parser/expand_utils.c parser/handlers.c parser/checker.c parser/groups.c
SRC_READER = reader/reader.c
SRC_UTILS = utils/utils.c utils/str.c
SRC = $(SRC_BUILTIN) $(SRC_EXEC) $(SRC_INIT) $(SRC_LEXER) $(SRC_PARSER) $(SRC_READER) $(SRC_UTILS)

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))
SRCS = $(addprefix $(SRCDIR)/, $(SRC))

# Rules of files
$(NAME): $(OBJS) $(LIBFTDIR)/libft.a
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFTDIR) -lft -L/opt/homebrew/opt/readline/lib -lreadline -o $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)/builtin $(OBJDIR)/exec $(OBJDIR)/init $(OBJDIR)/lexer $(OBJDIR)/parser $(OBJDIR)/reader $(OBJDIR)/utils

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
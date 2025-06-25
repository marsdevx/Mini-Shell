# Vars
CC = cc # cc / clang
CFLAGS = -I$(SRCDIR) -I$(LIBFTDIR) -I/opt/homebrew/opt/readline/include  -Wall -Wextra -Werror 
NAME = minishell
SRCDIR = src
OBJDIR = obj
LIBFTDIR = libs/libft

SRC_BUILTIN = builtin/builtin.c
SRC_EXEC = exec/exec.c exec/external.c exec/path.c exec/redirections.c exec/pipe.c
SRC_INIT = init/main.c
SRC_LEXER = lexer/lexer.c 
SRC_PARSER = parser/parser.c
SRC_READER = reader/reader.c
SRC_UTILS = utils/utils.c utils/env_utils.c
SRC = $(SRC_BUILTIN) $(SRC_EXEC) $(SRC_INIT) $(SRC_LEXER) $(SRC_PARSER) $(SRC_READER) $(SRC_UTILS)

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))
SRCS = $(addprefix $(SRCDIR)/, $(SRC))

# Rules of files
$(NAME): $(OBJS) $(LIBFTDIR)/libft.a
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFTDIR) -lft -L/opt/homebrew/opt/readline/lib -lreadline -o $(NAME)

# Create necessary directories for object files
$(OBJDIR):
	mkdir -p $(OBJDIR)/builtin $(OBJDIR)/exec $(OBJDIR)/init $(OBJDIR)/lexer $(OBJDIR)/parser $(OBJDIR)/reader $(OBJDIR)/utils

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rules to build the libraries
$(LIBFTDIR)/libft.a:
	$(MAKE) -C $(LIBFTDIR)

# Debug build target
debug: CFLAGS += -g
debug: re

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
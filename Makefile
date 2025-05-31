# Vars
CC = cc # cc / clang
CFLAGS = -I$(SRCDIR) -I$(LIBFTDIR) -I/opt/homebrew/opt/readline/include # -Wall -Werror -Wextra
NAME = minishell
SRCDIR = src
OBJDIR = obj
LIBFTDIR = libs/libft

SRC_DESTR =
SRC_EXEC = 
SRC_INIT = 
SRC_LEXER = lexer/lexer.c 
SRC_PARSER = parser/parser.c parser/temp_main.c
SRC_READER = reader/reader.c
SRC_UTILS = 
SRC = $(SRC_DESTR) $(SRC_EXEC) $(SRC_INIT) $(SRC_LEXER) $(SRC_PARSER) $(SRC_READER) $(SRC_UTILS)

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))
SRCS = $(addprefix $(SRCDIR)/, $(SRC))

# Rules of files
$(NAME): $(OBJS) $(LIBFTDIR)/libft.a
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFTDIR) -lft -L/opt/homebrew/opt/readline/lib -lreadline -o $(NAME)

# Create necessary directories for object files
$(OBJDIR):
	mkdir -p $(OBJDIR)/destr $(OBJDIR)/exec $(OBJDIR)/init $(OBJDIR)/lexer $(OBJDIR)/parser $(OBJDIR)/reader $(OBJDIR)/utils

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
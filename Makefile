# Vars
CC = cc # cc / clang
CFLAGS = -Wall -Werror -Wextra
NAME = minishell
SRCDIR = src
OBJDIR = obj
LIBFTDIR = libs/libft

SRC_CORE = core/main.c
SRC = $(SRC_CORE)

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))
SRCS = $(addprefix $(SRCDIR)/, $(SRC))

# Rules of files
$(NAME): $(OBJS) $(LIBFTDIR)/libft.a
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFTDIR) -lft -o $(NAME)

# Create necessary directories for object files
$(OBJDIR):
	mkdir -p $(OBJDIR)/core

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(SRCDIR) -I$(LIBFTDIR) -c $< -o $@

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
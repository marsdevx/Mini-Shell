# Vars
CC = cc # cc / clang
CFLAGS = -Wall -Werror -Wextra
NAME = minishell
SRCDIR = src
OBJDIR = obj
LIBFTDIR = libs/libft
FTPRINTF_DIR = libs/libftprintf

SRC_CORE = core/main.c
SRC = $(SRC_CORE)

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))
SRCS = $(addprefix $(SRCDIR)/, $(SRC))

# Rules of files
$(NAME): $(OBJS) $(LIBFTDIR)/libft.a $(FTPRINTF_DIR)/libftprintf.a
	$(CC) $(CFLAGS) $(OBJS) -L$(FTPRINTF_DIR) -lftprintf -L$(LIBFTDIR) -lft -o $(NAME)

# Create necessary directories for object files
$(OBJDIR):
	mkdir -p $(OBJDIR)/core

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(SRCDIR) -I$(LIBFTDIR) -I$(FTPRINTF_DIR) -c $< -o $@

# Rules to build the libraries
$(LIBFTDIR)/libft.a:
	$(MAKE) -C $(LIBFTDIR)

$(FTPRINTF_DIR)/libftprintf.a:
	$(MAKE) -C $(FTPRINTF_DIR)

# Debug build target
debug: CFLAGS += -g
debug: re

# Rules of actions
.PHONY: all clean fclean re

all: $(NAME)

clean:
	rm -rf $(OBJDIR)
	$(MAKE) -C $(LIBFTDIR) clean
	$(MAKE) -C $(FTPRINTF_DIR) clean

fclean: clean
	rm -rf $(NAME)
	$(MAKE) -C $(LIBFTDIR) fclean
	$(MAKE) -C $(FTPRINTF_DIR) fclean

re: fclean all
#ifndef READER_H
# define READER_H

# include "../init/init.h"

int		ft_init(t_info *info);
void	handle_sigint(int sig);
char	*ft_readline(const char *prompt);

#endif
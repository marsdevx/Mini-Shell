/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/06/25 23:40:02 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header.h"

static void	remove_argv_element(char **argv, int index)
{
	free(argv[index]);
	while (argv[index])
	{
		argv[index] = argv[index + 1];
		index++;
	}
}

int	setup_redirections(char ***argv_ptr)
{
	char	**argv;
	int		i;
	int		last_error;
	int		handled;

	argv = *argv_ptr;
	i = 0;
	last_error = 0;
	while (argv[i])
	{
		handled = 0;
		if (ft_strcmp(argv[i], "<") == 0)
		{
			if (!argv[i + 1])
			{
				fprintf(stderr, "syntax error: expected filename after '<'\n");
				return (-1);
			}
			if (handle_input_redirect(argv[i + 1]) < 0)
			{
				last_error = -1;
			}
			remove_argv_element(argv, i);
			remove_argv_element(argv, i);
			handled = 1;
		}
		else if (ft_strcmp(argv[i], ">") == 0)
		{
			if (!argv[i + 1])
			{
				fprintf(stderr, "syntax error: expected filename after '>'\n");
				return (-1);
			}
			if (handle_output_redirect(argv[i + 1]) < 0)
			{
				last_error = -1;
			}
			remove_argv_element(argv, i);
			remove_argv_element(argv, i);
			handled = 1;
		}
		else if (ft_strcmp(argv[i], ">>") == 0)
		{
			if (!argv[i + 1])
			{
				fprintf(stderr, "syntax error: expected filename after '>>'\n");
				return (-1);
			}
			if (handle_append_redirect(argv[i + 1]) < 0)
			{
				last_error = -1;
			}
			remove_argv_element(argv, i);
			remove_argv_element(argv, i);
			handled = 1;
		}
		else if (ft_strcmp(argv[i], "<<") == 0)
		{
			if (!argv[i + 1])
			{
				fprintf(stderr,
					"syntax error: expected delimiter after '<<'\n");
				return (-1);
			}
			if (handle_heredoc(argv[i + 1]) < 0)
			{
				last_error = -1;
			}
			remove_argv_element(argv, i);
			remove_argv_element(argv, i);
			handled = 1;
		}
		if (!handled)
			i++;
	}
	return (last_error);
}

int	handle_input_redirect(const char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "bash: %s: %s\n", filename, strerror(errno));
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

int	handle_output_redirect(const char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		fprintf(stderr, "bash: %s: %s\n", filename, strerror(errno));
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

int	handle_append_redirect(const char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		fprintf(stderr, "bash: %s: %s\n", filename, strerror(errno));
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

int	handle_heredoc(const char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;
		char *line;
	size_t	delim_len;
		int status;

	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return (-1);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	else if (pid == 0)
	{
		close(pipefd[0]);
		delim_len = ft_strlen(delimiter);
		while (1)
		{
			line = readline("> ");
			if (!line)
				break ;
			if (ft_strncmp(line, delimiter, delim_len) == 0
				&& line[delim_len] == '\0')
			{
				free(line);
				break ;
			}
			write(pipefd[1], line, ft_strlen(line));
			write(pipefd[1], "\n", 1);
			free(line);
		}
		close(pipefd[1]);
		exit(0);
	}
	else
	{
		close(pipefd[1]);
		waitpid(pid, &status, 0);
		if (dup2(pipefd[0], STDIN_FILENO) < 0)
		{
			perror("dup2");
			close(pipefd[0]);
			return -1;
		}
		close(pipefd[0]);
		return 0;
	}
}

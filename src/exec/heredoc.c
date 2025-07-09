/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkot <dkot@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:59:52 by dkot              #+#    #+#             */
/*   Updated: 2025/07/09 16:59:54 by dkot             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marksylaiev <marksylaiev@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:14:05 by dkot              #+#    #+#             */
/*   Updated: 2025/07/08 22:46:39 by marksylaiev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../init/header.h"

static void	heredoc_child_process(int write_fd, const char *delimiter)
{
	char	*line;
	size_t	delim_len;

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
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
}

static int	setup_heredoc_pipe(int *pipefd)
{
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

static int	fork_heredoc_process(void)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	return (pid);
}

static int	handle_heredoc_parent(int read_fd, pid_t pid)
{
	int	status;

	waitpid(pid, &status, 0);
	if (dup2(read_fd, STDIN_FILENO) < 0)
	{
		perror("dup2");
		close(read_fd);
		return (-1);
	}
	close(read_fd);
	return (0);
}

int	handle_heredoc(const char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;

	if (setup_heredoc_pipe(pipefd) < 0)
		return (-1);
	pid = fork_heredoc_process();
	if (pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	else if (pid == 0)
	{
		close(pipefd[0]);
		heredoc_child_process(pipefd[1], delimiter);
		close(pipefd[1]);
		exit(0);
	}
	else
	{
		close(pipefd[1]);
		return (handle_heredoc_parent(pipefd[0], pid));
	}
}

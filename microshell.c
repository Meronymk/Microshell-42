/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krochefo <krochefo@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/18 09:48:45 by krochefo          #+#    #+#             */
/*   Updated: 2023/04/20 19:13:59 by krochefo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void	perr(char *s)
{
	while (s)
		write(2, s++, 1);
}

int	cd(char **av, int i)
{
	if (i != 2)
		return (perr("error: bad arguments\n"), 1);
	else if (chdir(av[i]) == -1)
		return (perr("error: cannot change directory to "), perr(av[1]), perr("\n"), 1);
	return (0);
}

int	execmd(char **av, char **env, int i)
{
	int	status;
	int	fd[2];
	int	pip = (av[i] && !strcmp(av[i], "|"));

	if (pipe(fd) == -1)
		return (perr("error: fatal\n"), 1);
	int	pid = fork();
	if (!pid)
	{
		av[i] = 0;
		if (pip && (dup2(fd[1], 1) == -1 || close(fd[0]) || close(fd[1])))
			return (perr("error: fatal\n"), 1);
		execve(*av, av, env);
		return (perr("error: cannot execute "), perr(*av), perr("\n"), 1);
	}
	waitpid(pid, &status, 0);
	if (pip && (dup2(fd[0], 0) == -1 || close(fd[0]) || close(fd[1])))
		return (perr("error: fatal\n"), 1);
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	int	i = 0;
	int	status = 0;

	while (*av && *(av + 1))
	{
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		if (!strcmp(*av, "cd"))
			status = cd(av, i);
		else if (i)
			status = execmd(av, env, i);
		av += i;
	}
	return (status);
}

#include "minishell.h"

extern t_all	g_all;

void	close_fd(int **fd)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < g_all.args->elements - 1)
	{
		j = 0;
		while (j < 2)
		{
			if (fd[i][j] > 0)
				close(fd[i][j]);
			j++;
		}
		i++;
	}
	if (g_all.fd_in > 0)
		close(g_all.fd_in);
	if (g_all.fd_out > 0)
		close(g_all.fd_out);
	g_all.fd_in = 0;
	g_all.fd_out = 0;
}

int	**create_fd(int num)
{
	int	**fd;
	int	i;
	int	j;

	i = 0;
	j = 0;
	fd = malloc(sizeof(int *) * (num));
	if (!fd)
		return (0);
	while (i < num - 1)
	{
		fd[i] = malloc(sizeof(int) * 2);
		if (!fd[i])
		{
			while (j < i)
				free(fd[j++]);
			free(fd);
			return (0);
		}
		fd[i][0] = 0;
		fd[i][1] = 0;
		i++;
	}
	fd[num - 1] = NULL;
	return (fd);
}

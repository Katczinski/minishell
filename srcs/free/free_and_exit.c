#include "minishell.h"

extern t_all	g_all;

void	free_darr(void **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array && array[i])
	{
		free(array[i]);
		i++;
	}
	if (array)
		free(array);
}

void	ft_free(void)
{
	int				i;
	t_command_list	*temp;

	temp = 0;
	while (g_all.args && g_all.args->head)
	{
		i = 0;
		while (g_all.args->head
			&& g_all.args->head->command
			&& g_all.args->head->command[i])
			free(g_all.args->head->command[i++]);
		if (g_all.args->head->command)
			free(g_all.args->head->command);
		temp = g_all.args->head;
		g_all.args->head = g_all.args->head->next;
		free(temp);
	}
	if (g_all.args)
		free(g_all.args);
	free_darr((void **)g_all.path);
	g_all.path = 0;
	g_all.args = 0;
}

void	free_and_exit(struct stat *stats, int **fd, int status)
{
	close_fd(fd);
	free_darr((void **)fd);
	ft_free();
	free(stats);
	exit(status);
}

void	exit_child(t_command_list *cmd, int **fd)
{
	struct stat	*stats;
	int			ret;
	int			dir;

	stats = (struct stat *)malloc(sizeof(struct stat));
	dup2(g_all.std_in, STDIN_FILENO);
	dup2(g_all.std_out, STDOUT_FILENO);
	ret = stat(cmd->command[0], stats);
	dir = S_ISDIR(stats->st_mode);
	if (ft_strchr(cmd->command[0], '/') == NULL)
	{
		printf("%s: command not found\n", cmd->command[0]);
		free_and_exit(stats, fd, 127);
	}
	else if (ret && !dir)
	{
		printf("minishell: %s: no such file or directory\n",
			cmd->command[0]);
		free_and_exit(stats, fd, 127);
	}
	else if (!ret && dir)
		printf("minishell: %s: is a directory\n", cmd->command[0]);
	else if (!ret && !dir)
		printf("minishell: permission denied: %s\n", cmd->command[0]);
	free_and_exit(stats, fd, 126);
}

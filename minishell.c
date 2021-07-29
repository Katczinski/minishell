/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <abirthda@student.21-schoo>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/09 13:17:24 by abirthda          #+#    #+#             */
/*   Updated: 2021/07/28 19:44:47 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_all	g_all;

void	set_status(int status)
{
	if (WIFEXITED(status))
		g_all.exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		g_all.exit_status = 128 + WTERMSIG(status);
}

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

void	redir_and_exec(t_command_list *cmd)
{
	int			**fd;
	int			i;
	pid_t		pid;

	pid = -1;
	i = 0;
	fd = 0;
	if (g_all.args->elements > 1)
		fd = create_fd(g_all.args->elements);
	exec_dredin(cmd);
	if (next_pipe(cmd))
		g_all.exit_status = ft_pipe(cmd, fd, i);
	else
	{
		handle_redir(cmd);
		if (cmd && cmd->type == COMMAND && g_all.exec)
			pid = fork();
		exec_single_cmd(cmd, pid, fd);
	}
	close_fd(fd);
	free(g_all.binary);
	g_all.binary = 0;
	if (fd)
		free_darr((void **)fd);
}

void	execute(void)
{
	t_command_list	*cmd;
	struct stat		*stats;

	stats = malloc(sizeof(struct stat));
	set_term();
	cmd = find_cmd(g_all.args->head);
	if (!cmd)
	{
		printf("minishell: : command not found\n");
		g_all.exit_status = 127;
	}
	else
		redir_and_exec(cmd);
	if (!stat(".heredoc", stats))
		unlink(".heredoc");
	free(stats);
	dup2(g_all.std_in, STDIN_FILENO);
	dup2(g_all.std_out, STDOUT_FILENO);
	close(g_all.std_in);
	close(g_all.std_out);
	ft_free();
	tcsetattr(STDIN_FILENO, TCSANOW, &g_all.term);
}

void	parse_and_execute(char **line)
{
	add_history(*line);
	g_all.args = parser(*line, g_all.envp, g_all.exit_status);
	if (g_all.args)
		execute();
	else
	{
		free(*line);
		g_all.exit_status = 2;
	}
}

void	loop(void)
{
	char	*line;

	using_history();
	while (g_all.run_status == 0)
	{
		signal(SIGINT, sigint_handler);
		signal(SIGQUIT, SIG_IGN);
		g_all.exec = 1;
		line = readline("minishell> ");
		if (!line)
		{
			printf("exit\n");
			tcsetattr(STDIN_FILENO, TCSANOW, &g_all.saved);
			exit(0);
		}
		if (line[0] != '\0' && !is_all_whitespaces(line))
			parse_and_execute(&line);
		else
			free(line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	char	*term_name;

	(void)argc;
	(void)argv;
	tcgetattr(STDIN_FILENO, &g_all.saved);
	tcgetattr(STDIN_FILENO, &g_all.term);
	term_name = "xterm-256color";
	tcgetattr(0, &g_all.term);
	g_all.envp = save_envp(envp);
	g_all.run_status = 0;
	g_all.path = 0;
	rl_event_hook = event;
	g_all.term.c_lflag &= ~(ISIG);
	g_all.term.c_lflag &= ~(ECHOCTL);
	tcsetattr(0, TCSANOW, &g_all.term);
	tgetent(0, term_name);
	loop();
	free_darr((void **)g_all.envp);
	tcsetattr(STDIN_FILENO, TCSANOW, &g_all.saved);
	return (g_all.exit_status);
}

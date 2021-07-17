/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <abirthda@student.21-schoo>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/09 13:17:24 by abirthda          #+#    #+#             */
/*   Updated: 2021/06/09 18:04:18 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>
#include <sys/wait.h>

t_all	g_all;

void	ft_free(void)
{
	int		i;
	t_command_list	*temp;

	temp = 0;
	while (g_all.args && g_all.args->head)
	{
		i = 0;
		while (g_all.args->head->command
			&& g_all.args->head->command[i])
			free(g_all.args->head->command[i++]);
		if (g_all.args->head->command)
			free(g_all.args->head->command);
		if (g_all.binary)
			free(g_all.binary);
		temp = g_all.args->head;
		g_all.args->head = g_all.args->head->next; 
		free(temp);
	}
	free(g_all.args);
	g_all.args = 0;
}

int	event(void)
{
	return 0;
}

int	ft_putchar(int c)
{
	return (write(1, &c, 1));
}

void	sigint_handler(int signo)
{
	if (signo == SIGINT)
	{
		rl_redisplay();
		rl_replace_line("", 0);
		rl_done = 1;
	}
}

void	get_binary(t_command_list *cmd)
{
	char		*temp;
	int		i;
	struct stat	*stats;

	stats = (struct stat*)malloc(sizeof(struct stat));
	i = 0;
	if (g_all.binary)
		free(g_all.binary);
	while (g_all.path[i])
	{
		temp = ft_strjoin(g_all.path[i], "/");
		if (!temp)
			exit(1);
		g_all.binary = ft_strjoin(temp, cmd->command[0]);
		free(temp);
		if (!stat(g_all.binary, stats))
			break ;
		free(g_all.binary);
		g_all.binary = 0;
		i++;
	}
	free(stats);
	if (!g_all.path[i] && cmd->command[0])
		printf("%s: command not found\n", cmd->command[0]);
}

void	close_fd(int (*fd)[2])
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
			close(fd[i][j]);
			j++;
		}
		i++;
	}
}

int	find_redirin(t_command_list *cmd, int type,int flags)
{
	int	fd;
	
	
	while (cmd->prev && cmd->prev->type != PIPE)
		cmd = cmd->prev;
	while (cmd && cmd->type != PIPE)
	{
		if (cmd->type == type)
		{
			fd = open(cmd->command[0], flags, 0644);
			if (fd < 0)
			{
				printf("cat: %s: No such file or directory\n",
				cmd->command[0]);
				return (-1);
			}
			if (cmd->next == NULL || cmd->next->type == PIPE )
				return (fd);
			if (cmd->next->type != COMMAND)
				close(fd);
		}
		cmd = cmd->next;
	}
	if (fd)
		return (fd);
	return (0);

}

int	next_cmd(t_command_list *cmd)
{
	while (cmd && cmd->type != PIPE)
		cmd = cmd->next;
	if (cmd && cmd->type == PIPE)
		return (1);
	return (0);
}

void	child(int (*fd)[2], t_command_list *cmd, int *pid, int i, char **envp)
{
	int		fd_in;
	int		fd_out;
	
	fd_in = 0;
	pid[i] = fork();
		if (pid[i] == 0)
		{
			if (i != 0)
				dup2(fd[i - 1][0], STDIN_FILENO);
			fd_in = find_redirin(cmd, RED_IN, O_RDONLY);
			if (fd_in >= 0)
				dup2(fd_in, STDIN_FILENO);
			else
				exit(EXIT_FAILURE);
			if (next_cmd(cmd))
				dup2(fd[i][1], STDOUT_FILENO);
			fd_out = find_redirin(cmd, RED_OUT, O_WRONLY | O_CREAT | O_TRUNC);
			if (fd_out > 0)
				dup2(fd_out, STDOUT_FILENO);
			else if (fd_out < 0)
				exit(EXIT_FAILURE);
			if (fd_out)
				close(fd_out);
			close_fd(fd);
			execve(g_all.binary, cmd->command, envp);
		}
}


int	execute(char **envp)
{
	
	(void)envp;
	t_command_list	*cmd;
	cmd = g_all.args->head;
	int	pid[g_all.args->elements];
	int	fd[g_all.args->elements - 1][2];
	int	i = 0;
	for (int j = 0; j < g_all.args->elements; j++)
	{
		pid[j] = 0;
		if (j < g_all.args->elements - 1)
			pipe(fd[j]);
	}
	while (cmd)
	{
		if (cmd->type == COMMAND)
			get_binary(cmd);
		if (g_all.binary && cmd->type == COMMAND)
		{
			child(fd, cmd, pid, i, envp);
			free(g_all.binary);
			g_all.binary = 0;
			i++;
		}
		cmd = cmd->next;
	}
	close_fd(fd);
	for (int j = 0; j < g_all.args->elements; j++)
		waitpid(pid[j], 0, 0);

	return (1);	

}

void	loop(int argc, char **argv, char **envp)
{
	char	*line;
	(void)argc;
	(void)argv;
	(void)envp;
	using_history();
	while (g_all.status)
	{
		signal(SIGINT, sigint_handler);
		signal(SIGQUIT, SIG_IGN);
		line = readline("minishell> ");
		if (!line)
		{
    			tcsetattr(STDIN_FILENO, TCSANOW, &g_all.saved);
			exit(1);
		}
		if (line[0] != '\0')
		{
			add_history(line);
			g_all.args = parser(line, envp);
			// printf("here\n");
			if (g_all.args)
			{
				g_all.status = execute(envp);
				ft_free();
			}
		}
//		free(line);  // double free
	}
}

char	**get_path(char **envp)
{
	char	**path;

	while (*envp != NULL && ft_strncmp(*envp, "PATH=", 5))
		envp++;
	if (envp == NULL)
		return (0);
	path = ft_split(*envp + 5, ':');
	return (path);
}

int	main(int argc, char **argv, char **envp)
{
	char	*term_name;
	

	tcgetattr(STDIN_FILENO, &g_all.saved);
	term_name = "xterm-256color";
	tcgetattr(0, &g_all.term);
	g_all.path = get_path(envp);
	if (g_all.path == 0)
		return (0);
	g_all.status = 1;
	rl_event_hook = event;
	g_all.term.c_lflag &= ~(ISIG);	
	g_all.term.c_lflag &= ~(ECHOCTL);
	tcsetattr(0, TCSANOW, &g_all.term);
	tgetent(0, term_name);
	loop(argc, argv, envp);
	return (1);
}

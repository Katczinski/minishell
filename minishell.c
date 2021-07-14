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

	while (g_all.args && g_all.args->head)
	{
		i = 0;
		while (g_all.args->head &&  g_all.args->head->command[i])
			free(g_all.args->head->command[i++]);
		free(g_all.args->head->command);
		free(g_all.args->head->file_name);
		temp = g_all.args->head->next;
		free(g_all.args->head);
		g_all.args->head = temp;
	}
	free(g_all.args);
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

void	child(int (*fd)[2], t_command_list *cmd, int *pid, int i, char **envp)
{
	pid[i] = fork();
		if (pid[i] == 0)
		{
			if (i != 0)
				dup2(fd[i - 1][0], STDIN_FILENO);
			if (cmd->next != NULL)
				dup2(fd[i][1], STDOUT_FILENO);
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
		get_binary(cmd);
		if (g_all.binary)
			child(fd, cmd, pid, i, envp);
		i++;
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
			g_all.status = execute(envp);
			free(line);
		}
		ft_free();
		
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

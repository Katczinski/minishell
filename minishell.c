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

int	redir_out(t_command_list *cmd)
{
	int	fd;
	
	fd = 0;
	while (cmd && cmd->type != PIPE)
	{
		if (cmd->type == RED_OUT || cmd->type == DRED_OUT)
		{
			if (fd)
				close(fd);
			if (cmd->type == RED_OUT)
				fd = open(cmd->command[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			else
				fd = open(cmd->command[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd < 0)
			{
				printf("cat: %s: No such file or directory\n",
				cmd->command[0]);
				return (-1);
			}
			if (cmd->next == NULL || cmd->next->type == PIPE )
				return (fd);
		}
		cmd = cmd->next;
	}
	if (fd)
		return (fd);
	return (0);

}

int	redir_in(t_command_list *cmd)
{
	int	fd;
	
	fd = 0;
	while (cmd && cmd->type != PIPE)
	{
		if (cmd->type == RED_IN)
		{
			if (fd)
				close(fd);
			fd = open(cmd->command[0], O_RDONLY, 0644);
			if (fd < 0)
			{
				printf("cat: %s: No such file or directory\n",
				cmd->command[0]);
				return (-1);
			}
			if (cmd->next == NULL || cmd->next->type == PIPE )
				return (fd);
		}
		cmd = cmd->next;
	}
	if (fd)
		return (fd);
	return (0);
}

int	find_redir(t_command_list *cmd, int type)
{
	int fd;

	fd = 0;
	while (cmd->prev && cmd->prev->type != PIPE)
		cmd = cmd->prev;
	if (type == RED_OUT)
		fd = redir_out(cmd);
	else if (type == RED_IN)
		fd = redir_in(cmd);
	return (fd);
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
	int		std_in;
	int		std_out;

	std_in = dup(STDIN_FILENO);
	std_out = dup(STDOUT_FILENO);	
	fd_in = 0;
	fd_out = 0;
	fd_in = find_redir(cmd, RED_IN);
	if (fd_in >= 0)
		dup2(fd_in, STDIN_FILENO);
	else
		return ;
	fd_out = find_redir(cmd, RED_OUT);
	if (fd_out > 0)
		dup2(fd_out, STDOUT_FILENO);
	else if (fd_out < 0)
		return ;
	if (fd_in)
		close(fd_in);
	if (fd_out)
		close(fd_out);
	pid[i] = fork();
	if (pid[i] == 0)
	{
		if (cmd->type != COMMAND)
			exit(1);
		if (i != 0 && fd_in == 0)
			dup2(fd[i - 1][0], STDIN_FILENO);
		if (next_cmd(cmd))
			dup2(fd[i][1], STDOUT_FILENO);
		close_fd(fd);
		execve(g_all.binary, cmd->command, envp);
		exit(1);
	}
	else if (pid[i])
	{
		if (i != 0 && fd_in == 0)
			dup2(fd[i - 1][0], STDIN_FILENO);
		if (next_cmd(cmd))
			dup2(fd[i][1], STDOUT_FILENO);
		if (cmd->type == FT_ECHO)
			ft_echo(cmd);
		if (cmd->type == FT_PWD)
			ft_pwd(g_all.args);
		dup2(std_in, STDIN_FILENO);
		dup2(std_out, STDOUT_FILENO);
	}
}


int	execute(char **envp)
{
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
		if ((g_all.binary && cmd->type == COMMAND) || cmd->type == FT_ECHO || cmd->type == FT_PWD)
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

void	loop(char **envp)
{
	char	*line;
	
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
			// printf("parsing...\n");
			g_all.args = parser(line, envp);
			// printf("executing...\n");
			if (g_all.args)
			{
				g_all.status = execute(envp);
				ft_free();
			}
		}
	//	free(line);  // double free
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
	(void)argc;
	(void)argv;	

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
	loop(envp);
	return (1);
}

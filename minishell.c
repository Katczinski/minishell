/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <abirthda@student.21-schoo>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/09 13:17:24 by abirthda          #+#    #+#             */
/*   Updated: 2021/07/19 17:51:15 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_all	g_all;

int	is_builtin(int type)
{
	if (type == FT_ECHO || type == FT_PWD || type == FT_CD
	|| type == FT_EXPORT || type == FT_UNSET || type == FT_ENV
	|| type == FT_EXIT || type == DRED_IN)
		return (1);
	return (0);
}


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

int	next_cmd(t_command_list *cmd)
{
	while (cmd && cmd->type != PIPE)
		cmd = cmd->next;
	if (cmd && cmd->type == PIPE)
		return (1);
	return (0);
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

int	dredin(t_command_list *cmd)
{
	char	*line;

	if (g_all.fd_in > 0)
		close(g_all.fd_in);
	g_all.fd_in = open(".heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (g_all.fd_in == -1)
	{
		printf("%s: No such file or directory\n",
			cmd->command[0]);
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, cmd->command[0]))
		{
			ft_putendl_fd(line, g_all.fd_in);
			free(line);
		}
		else
		{
			free(line);
			close(g_all.fd_in);
			g_all.fd_in = open(".heredoc", O_RDONLY, 0644);
			return (1);
		}
	}
	close(g_all.fd_in);
	g_all.fd_in = open(".heredoc", O_RDONLY, 0644);
	return (1);
}
int	redin(t_command_list *cmd)
{

	if (g_all.fd_in > 0)
		close(g_all.fd_in);
	g_all.fd_in = open(cmd->command[0], O_RDONLY, 0644);
	if (g_all.fd_in == -1)
	{
		printf("%s: No such file or directory\n",
			cmd->command[0]);
		return (-1);
	}
	return (1);
}

int	redout(t_command_list *cmd)
{

	if (g_all.fd_out > 0)
		close(g_all.fd_out);
	if (cmd->type == RED_OUT)
		g_all.fd_out = open(cmd->command[0],
		O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		g_all.fd_out = open(cmd->command[0],
		O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (g_all.fd_out == -1)
	{
		printf("%s: No such file or directory\n",
			cmd->command[0]);
		return (-1);
	}
	return (1);
}
/*
void	exec_bin(int (*fd)[2], int pid, int i, char **envp)
{
	struct stat	*stats;
	
	stats = (struct stat*)malloc(sizeof(struct stat));

	pid = fork();
	if (pid == 0)
	{
		if (i != 0 && g_all.fd_in == 0)
			dup2(fd[i - 1][0], STDIN_FILENO);
		if (next_cmd(g_all.cmd) && g_all.fd_out == 0)
			dup2(fd[i][1], STDOUT_FILENO);
		close_fd(fd);
		if (g_all.fd_in > 0)
		{
			dup2(g_all.fd_in, STDIN_FILENO);
			close(g_all.fd_in);
		}
		if (g_all.fd_out > 0)
		{
			dup2(g_all.fd_out, STDOUT_FILENO);
			close(g_all.fd_out);
		}
		if (g_all.binary && g_all.cmd)
			execve(g_all.binary, g_all.cmd->command, envp);
		else
			exit(1);
	}
	else
	{
		if (g_all.fd_in > 0)
			close(g_all.fd_in);
		if (g_all.fd_out > 0)
			close(g_all.fd_out);
		g_all.fd_in = 0;
		g_all.fd_out = 0;
		if (!stat(".heredoc", stats))
			unlink(".heredoc");
		free(stats);
	}
}

void	exec_builtin(int (*fd)[2], int i, char **envp)
{

	struct stat	*stats;
	
	stats = (struct stat*)malloc(sizeof(struct stat));
	if (i != 0 && g_all.fd_in == 0)
		dup2(fd[i - 1][0], STDIN_FILENO);
	if (next_cmd(g_all.cmd) && g_all.fd_out == 0)
		dup2(fd[i][1], STDOUT_FILENO);
	if (g_all.fd_in > 0)
	{
		dup2(g_all.fd_in, STDIN_FILENO);
		close(g_all.fd_in);
	}
	if (g_all.fd_out > 0)
	{
		dup2(g_all.fd_out, STDOUT_FILENO);
		close(g_all.fd_out);
	}
	g_all.fd_in = 0;
	g_all.fd_out = 0;
	if (g_all.cmd->type == FT_ECHO)
		ft_echo(g_all.cmd);
	if (g_all.cmd->type == FT_PWD)
		ft_pwd(g_all.args);
	if (g_all.cmd->type == FT_CD)
		ft_cd(g_all.args->head, envp, g_all.args);
	if (g_all.fd_in > 0)
		close(g_all.fd_in);
	if (g_all.fd_out > 0)
		close(g_all.fd_out);
	close_fd(fd);
	if (!stat(".heredoc", stats))
		unlink(".heredoc");
	free(stats);
}
*/

void	exec_builtin(int (*fd)[2], int i, char **envp)
{
	if (i != 0 && g_all.fd_in == 0)
		dup2(fd[i - 1][0], STDIN_FILENO);
	if (next_cmd(g_all.cmd))
		dup2(fd[i][1], STDOUT_FILENO);
	if (g_all.cmd->type == FT_ECHO)
		g_all.exit_status = ft_echo(g_all.cmd);
	if (g_all.cmd->type == FT_PWD)
		g_all.exit_status = ft_pwd(g_all.args);
	if (g_all.cmd->type == FT_CD)
		g_all.exit_status = ft_cd(g_all.cmd, envp, g_all.args);
	if (g_all.cmd->type == FT_EXPORT)
		g_all.exit_status = ft_export(g_all.cmd, &g_all.envp, g_all.args);
	if (g_all.cmd->type == FT_UNSET)
		g_all.exit_status = ft_unset(g_all.cmd, &g_all.envp, g_all.args);
	if (g_all.cmd->type == FT_ENV)
		g_all.exit_status = ft_env(g_all.envp);
	if (g_all.cmd->type == FT_EXIT)
		ft_exit(g_all.cmd, &g_all.exit_status, &g_all.status);
}

void	exec_bin(int (*fd)[2], int i, char **envp)
{
	if (i != 0 && g_all.fd_in == 0)
		dup2(fd[i - 1][0], STDIN_FILENO);
	if (next_cmd(g_all.cmd) && g_all.fd_out == 0)
		dup2(fd[i][1], STDOUT_FILENO);
	close_fd(fd);
	if (g_all.fd_in > 0)
	{
		dup2(g_all.fd_in, STDIN_FILENO);
		close(g_all.fd_in);
	}
	if (g_all.fd_out > 0)
	{
		dup2(g_all.fd_out, STDOUT_FILENO);
		close(g_all.fd_out);
	}
	if (g_all.binary && g_all.cmd)
		execve(g_all.binary, g_all.cmd->command, envp);
	else
		exit(1);

}
void	exec(int (*fd)[2], int pid, int i, char **envp)
{
	int		std_in;
	int		std_out;
	struct stat	*stats;
	
	stats = (struct stat*)malloc(sizeof(struct stat));
	std_in = dup(STDIN_FILENO);
	std_out = dup(STDOUT_FILENO);	
	pid = fork();
	if (pid == 0)
		exec_bin(fd, i, envp);
	else if (pid)
	{
		exec_builtin(fd, i, envp);
		dup2(std_in, STDIN_FILENO);
		dup2(std_out, STDOUT_FILENO);
		close(std_in);
		close(std_out);
		if (g_all.fd_in)
			close(g_all.fd_in);
		if (g_all.fd_out)
			close(g_all.fd_out);
		if (!stat(".heredoc", stats))
			unlink(".heredoc");
		free(stats);
		if (g_all.binary)
		{
			free(g_all.binary);
			g_all.binary = 0;
		}
	}
}

int	exec_node(t_command_list *cmd, int (*fd)[2], int pid, int i, char **envp)
{
	if (cmd->type == RED_IN)
	{
		if (redin(cmd) == -1)
			return (-1);
	}
	else if (cmd->type == DRED_IN)
	{
		if (dredin(cmd) == -1)
			return (-1);
	}
	else if (cmd->type == RED_OUT || cmd->type == DRED_OUT)
	{
		if (redout(cmd) == -1)
			return (-1);
	}
	
	if (cmd->type == COMMAND || is_builtin(cmd->type))
		g_all.cmd = cmd;
	if ((cmd->type == PIPE || cmd->next == NULL))
	{
		exec(fd, pid, i, envp);
//		if (g_all.cmd && g_all.cmd->type == COMMAND)
//			exec_bin(fd, pid, i, envp);
//		else if (g_all.cmd && is_builtin(g_all.cmd->type))
//			exec_builtin(fd, i, envp);
	}
	return (1);
}

int	execute(char **envp)
{
	t_command_list	*cmd;
	int	pid[g_all.args->elements];
	int	fd[g_all.args->elements - 1][2];
	int	i = 0;
	
	cmd = g_all.args->head;
	g_all.fd_in = 0;
	g_all.fd_out = 0;

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
		if (exec_node(cmd, fd, pid[i], i, envp) == -1)
			break ;
		if (cmd->type == PIPE)
		{
			i++;
			if (g_all.fd_in > 0)
				close(g_all.fd_in);
			if (g_all.fd_out > 0)
				close(g_all.fd_out);
			g_all.fd_in = 0;
			g_all.fd_out = 0;
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
	while (!g_all.status)
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
//			printf("parsing...\n");
			g_all.args = parser(line, envp, g_all.exit_status);
//			printf("executing...\n");
			if (g_all.args)
			{
				execute(g_all.envp);
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

char **save_envp(char **envp)
{
	int i;
	char **new_envp;

	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * (i + 1));
	new_envp[i] = 0;
	i = -1;
	while (envp[++i])
		new_envp[i] = strdup(envp[i]);
	// envp++;
	// new_envp = malloc(sizeof(char *) * (4));
	// new_envp[3] = 0;
	// new_envp[0] = strdup("ddd=1");
	// new_envp[1] = strdup("ccc=2");
	// new_envp[2] = strdup("PATH=/home");

	return (new_envp);
}

int	main(int argc, char **argv, char **envp)
{
	char	*term_name;
	(void)argc;
	(void)argv;

	tcgetattr(STDIN_FILENO, &g_all.saved);
	term_name = "xterm-256color";
	tcgetattr(0, &g_all.term);
	g_all.envp = save_envp(envp);
	g_all.path = get_path(g_all.envp);
	if (g_all.path == 0)
		return (0);
	g_all.status = 0;
	rl_event_hook = event;
	g_all.term.c_lflag &= ~(ISIG);	
	g_all.term.c_lflag &= ~(ECHOCTL);
	tcsetattr(0, TCSANOW, &g_all.term);
	tgetent(0, term_name);
	loop(envp);
	return (g_all.exit_status);
}

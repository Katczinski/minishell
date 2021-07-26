/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <abirthda@student.21-schoo>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/09 13:17:24 by abirthda          #+#    #+#             */
/*   Updated: 2021/07/22 19:03:44 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_all	g_all;

int	is_builtin(int type)
{
	if (type == FT_ECHO || type == FT_PWD || type == FT_CD
	|| type == FT_EXPORT || type == FT_UNSET || type == FT_ENV
	|| type == FT_EXIT)
		return (1);
	return (0);
}

int		is_redir(int type)
{
	if (type == RED_IN || type == DRED_IN ||
		type == RED_OUT || type == DRED_OUT)
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
		g_all.status = 130;
		rl_redisplay();
		rl_replace_line("", 0);
		rl_done = 1;
	}
}

char	**get_path(char **envp)
{
	char	**path;
	
	while (*envp != NULL && ft_strncmp(*envp, "PATH=", 5))
		envp++;

	if (*envp == NULL)
		return (0);
	path = ft_split(*envp + 5, ':');
	return (path);
}

void	get_binary(t_command_list *cmd)
{
	char		*temp;
	int			i;
	struct stat	*stats;

	stats = (struct stat*)malloc(sizeof(struct stat));
	i = 0;
	while (g_all.path && g_all.path[i])
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
	if (!g_all.binary)
	{
		if (!stat(cmd->command[0], stats) && !(S_ISDIR(stats->st_mode)))
			g_all.binary = ft_strdup(cmd->command[0]);
	}
	else if (g_all.path && !g_all.path[i] && cmd->command[0])
		printf("%s: command not found\n", cmd->command[0]);
	free(stats);
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



// t_command_list	*find_cmd(t_command_list *cmd)
// {
// 	t_command_list	*next;

// 	next = cmd;
// 	while (next && (next->type != COMMAND && !is_builtin(next->type)))
// 		next = next->next;
// 	if (next && (next->type == COMMAND || is_builtin(next->type)))
// 		return (next);
// 	else
// 		return (cmd);
// }

t_command_list	*get_cmd(t_command_list *cmd)
{
	t_command_list	*next;

	next = cmd;
	while (next && (next->type != COMMAND
		&& !is_builtin(next->type) && next->type != PIPE))
		next = next->next;
	if (next && (next->type == COMMAND || is_builtin(next->type)))
		return (next);
	return (0);
}

t_command_list	*next_pipe(t_command_list *cmd)
{
	t_command_list	*next = 0;

	next = cmd;
	while (next && next->type != PIPE)	
		next = next->next;
	if (next && next->type == PIPE)
		return (next->next);
	return (0);
}

// int		prev_pipe(t_command_list *cmd)
// {
// 	t_command_list	*prev;

// 	prev = cmd;
// 	while (prev && prev->type != PIPE)
// 		prev = prev->prev;
// 	if (prev)
// 		return (1);
// 	return (0);
// }

void	exec_dredin(t_command_list *cmd)
{
	int	fd = 0;
	char *line;
	while (cmd)
	{
		if (cmd->type == DRED_IN)
		{
			fd = open(".heredoc", O_CREAT | O_WRONLY | O_TRUNC, 0644);
			while (1)
			{
				line = readline("> ");
				if (!line)
					break ;
				else if (line && ft_strcmp(line, cmd->command[0]))
				{
					ft_putendl_fd(line, fd);
					free(line);
				}
				else
				{	
					close(fd);
					break ;
				}
			}		
		}
		cmd = cmd->next;
	}
	if (fd)
		close(fd);
}

void	handle_redir(t_command_list *cmd)
{
	int				fd_in = 0;
	int				fd_out = 0;

	while (cmd->prev && cmd->prev->type != PIPE)
		cmd = cmd->prev;
	while (cmd && cmd->type != PIPE)
	{
		if (is_redir(cmd->type))
		{

			if (cmd->type == RED_IN || cmd->type == DRED_IN)
			{
				if (fd_in)
					close(fd_in);
				if (cmd->type == RED_IN)
					fd_in = open(cmd->command[0], O_RDONLY, 0644);
				else if (cmd->type == DRED_IN)
					fd_in = open(".heredoc", O_RDONLY, 0644);
			}
			else
			{		
				if (fd_out)
					close(fd_out);
				if (cmd->type == RED_OUT)
					fd_out = open(cmd->command[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				else if (cmd->type == DRED_OUT)
					fd_out = open(cmd->command[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
			}
			if (fd_in == -1 || fd_out == -1)
			{
				printf("%s: No such file or directory\n", cmd->command[0]);
				break ;
			}

		}
		cmd = cmd->next;
	}
	g_all.fd_in = fd_in;
	g_all.fd_out = fd_out;
	if (g_all.fd_in > 0)
		dup2(g_all.fd_in, STDIN_FILENO);
	if (g_all.fd_out > 0)
		dup2(g_all.fd_out, STDOUT_FILENO);
	if (g_all.fd_in == -1 || g_all.fd_out == -1)
		g_all.exec = 0;
	
}
// int		create_pipe(void)
// {
// 	pid_t		pid;
// 	int		fd[2];

// 	pipe(fd);
// 	pid = fork();
// 	if (pid == 0)
// 	{	
// 		if (fd[1])
// 			close(fd[1]);
// 		if (g_all.fd_in == 0)
// 		{
// 			printf("pipein");
// 			dup2(fd[0], STDIN_FILENO);
// 		}
// 		g_all.pipe_in = fd[0];
// 		return (2);
// 	}
// 	else
// 	{
// 		if (fd[0])
// 			close(fd[0]);
// 		if (g_all.fd_out == 0)
// 		{
// 			printf("pipeout");
// 			dup2(fd[1], STDOUT_FILENO);
// 		}
// 		g_all.pipe_in = fd[1];
// 		return (1);
// 	}

// }

void	exit_child(t_command_list *cmd, int (*fd)[2])
{

	struct stat	*stats;
	int		ret;
	int		dir;
	stats = (struct stat*)malloc(sizeof(struct stat));
	dup2(g_all.std_in, STDIN_FILENO);
	dup2(g_all.std_out, STDOUT_FILENO);
	close_fd(fd);
	ret = stat(cmd->command[0], stats); 
	dir = S_ISDIR(stats->st_mode);
	if (ft_strchr(cmd->command[0], '/') == NULL)
	{
		printf("%s: command not found\n", cmd->command[0]);
		exit(127);
	}
	else if (ret && !dir)
		printf("minishell: %s: no such file or directory\n", cmd->command[0]);
	else if (!ret && dir)
		printf("minishell: %s: is a directory\n", cmd->command[0]);
	else if (!ret && !dir) 
		printf("minishell: permission denied: %s\n", cmd->command[0]);
	exit(126);

}

void	exec_builtin(t_command_list *cmd)
{
		if (cmd->type == FT_ECHO)
			g_all.exit_status = ft_echo(cmd);
		if (cmd->type == FT_PWD)
			g_all.exit_status = ft_pwd(g_all.args);
		if (cmd->type == FT_CD)
			g_all.exit_status = ft_cd(cmd, g_all.envp, g_all.args);
		if (cmd->type == FT_EXPORT)
			g_all.exit_status = ft_export(cmd, &g_all.envp, g_all.args);
		if (cmd->type == FT_UNSET)
			g_all.exit_status = ft_unset(cmd, &g_all.envp, g_all.args);
		if (cmd->type == FT_ENV)
			g_all.exit_status = ft_env(g_all.envp);
		if (cmd->type == FT_EXIT)
			ft_exit(cmd, &g_all.exit_status, &g_all.status);
}

void	exec(t_command_list *cmd, int (*fd)[2])
{
	pid_t	pid = -1;
	
	if (!cmd)
		return ;
	g_all.path = get_path(g_all.envp);
	get_binary(cmd);
//	printf("%d\n", g_all.fd_in);
	if (cmd->type == COMMAND)
		pid = fork();
	if (pid == 0)
	{
//		close_fd(fd);
		if (g_all.binary)
		{
			if (execve(g_all.binary, cmd->command, g_all.envp) == -1)
				exit_child(cmd, fd);
		}
		else
			exit_child(cmd, fd);
		}
	else
	{
		if (is_builtin(cmd->type))
			exec_builtin(cmd);		
//		printf("here\n");
//		dup2(g_all.std_in, STDIN_FILENO);
//		dup2(g_all.std_out, STDOUT_FILENO);
		close_fd(fd);
		waitpid(pid, &g_all.exit_status, 0);
		g_all.status = WEXITSTATUS(g_all.status);
	}
}

void	ft_pipe(t_command_list *cmd, int (*fd)[2], int i)
{
	pid_t	pid;
//	char			command[256];
	if (!cmd)
		return ;
	pipe(fd[i]);
	pid = fork();
	if (pid == 0)
	{
//		sleep(20);
//		printf("%s\n", cmd->command[0]);
		handle_redir(cmd);
		if (is_redir(cmd->type))
			exit(1);
		if (i != 0 && g_all.fd_in == 0)
		{
//			printf("pipin %s\n", cmd->command[0]);
			dup2(fd[i - 1][0], STDIN_FILENO);
		}
		if (next_pipe(cmd) && g_all.fd_out == 0)
		{
//			printf("pipout %s\n", cmd->command[0]);
			dup2(fd[i][1], STDOUT_FILENO);
		}
		close_fd(fd);
		if (g_all.exec)
			exec(get_cmd(cmd), fd);	
		exit(1);
	}
	else
	{
		
//	close_fd(fd);
//		snprintf(command, sizeof(command), "code --open-url \"vscode://vadimcn.vscode-lldb/launch/config?{'request':'attach','pid':%d,'stopOnEntry':true}\"", pid);
//		system(command);
	}
	ft_pipe(next_pipe(cmd), fd, ++i);
	close_fd(fd);
	waitpid(pid, 0, 0);


}

void	redir_and_exec(t_command_list *cmd)
{
	// int				g_all.std_in;
	// int				g_all.std_out;
	// g_all.std_in = dup(STDIN_FILENO);
	// g_all.std_out = dup(STDOUT_FILENO);
	struct stat *stats;
	stats = malloc(sizeof(struct stat));
	int 	fd[g_all.args->elements - 1][2];
	int	i = 0;	
	exec_dredin(cmd);
	if (next_pipe(cmd))
		ft_pipe(cmd, fd, i);
	else
	{
		handle_redir(cmd);
		if (g_all.exec)
			exec(get_cmd(cmd), fd);
	}
	close_fd(fd);
	if (!stat(".heredoc", stats))
		unlink(".heredoc");
	free(g_all.binary);
	g_all.binary = 0;
	free(stats);
	dup2(g_all.std_in, STDIN_FILENO);
	dup2(g_all.std_out, STDOUT_FILENO);
//	close(g_all.std_in);
//	close(g_all.std_out);
}

void	execute(void)
{
	t_command_list	*cmd;
//	int				g_all.std_in;
//	int				g_all.std_out;
	g_all.std_in = dup(STDIN_FILENO);
	g_all.std_out = dup(STDOUT_FILENO);
	cmd = g_all.args->head;
//	cmd = find_cmd(g_all.args->head);
	
	redir_and_exec(cmd);
//	dup2(g_all.std_in, STDIN_FILENO);
//	dup2(g_all.std_out, STDOUT_FILENO);
	close(g_all.std_in);
	close(g_all.std_out);
	
}
void	loop(void)
{
	char	*line;
	
	using_history();
	
	while (1)
	{
		
		g_all.exec = 1;
		signal(SIGINT, sigint_handler);
		signal(SIGQUIT, SIG_IGN);
		line = readline("minishell> ");
		if (g_all.path)
			free(g_all.path);
		g_all.path = get_path(g_all.envp);
		if (!line)
		{
    			tcsetattr(STDIN_FILENO, TCSANOW, &g_all.saved);
			exit(1);
		}
		if (line[0] != '\0')
		{
			add_history(line);
//			printf("parsing...\n");
			g_all.args = parser(line, g_all.envp, g_all.exit_status);
			// printf("executing...\n");
			if (g_all.args)
			{
				execute();
				ft_free();
			}
		}
	//	free(line);  // double free
	}
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
		new_envp[i] = ft_strdup(envp[i]);
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
	g_all.status = 0;
//	g_all.std_in = dup(STDIN_FILENO);
//	g_all.std_out = dup(STDOUT_FILENO);
//	g_all.path = get_path(g_all.envp);
//	if (g_all.path == 0)
//		return (0);
//  g_all.status = 0;
	rl_event_hook = event;
	g_all.term.c_lflag &= ~(ISIG);	
	g_all.term.c_lflag &= ~(ECHOCTL);
	tcsetattr(0, TCSANOW, &g_all.term);
	tgetent(0, term_name);
	loop();
	return (g_all.exit_status);
}

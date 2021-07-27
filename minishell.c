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

int	is_redir(int type)
{
	if (type == RED_IN || type == DRED_IN
		|| type == RED_OUT || type == DRED_OUT)
		return (1);
	return (0);
}

void	ft_free(void)
{
	int				i;
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
	return (0);
}

int	ft_putchar(int c)
{
	return (write(1, &c, 1));
}

void	sigint_handler(int signo)
{
	if (signo == SIGINT)
	{
		g_all.exit_status = 130;
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

	stats = (struct stat *)malloc(sizeof(struct stat));
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

t_command_list	*get_cmd(t_command_list *cmd)
{
	while (cmd && (cmd->type != COMMAND
			&& !is_builtin(cmd->type) && cmd->type != PIPE))
		cmd = cmd->next;
	if (cmd && (cmd->type == COMMAND || is_builtin(cmd->type)))
		return (cmd);
	return (0);
}

t_command_list	*next_pipe(t_command_list *cmd)
{
	while (cmd && cmd->type != PIPE)
		cmd = cmd->next;
	if (cmd && cmd->type == PIPE)
		return (cmd->next);
	return (0);
}

char	*subst_value(char *line)
{
	int	i;

	i = 0;
	if (ft_strchr(line, '$') == NULL)
		return (line);
	while (line && line[i] != '\0')
	{
		if (line[i] == '$')
			line = treat_env(line, &i, g_all.envp, g_all.args);
		i++;
	}
	return (line);
}

void	exec_dredin(t_command_list *cmd)
{
	int		fd = 0;
	char	*line;

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
					if (!cmd->quoted)
						line = subst_value(line);
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
					fd_out = open(cmd->command[0],
							O_WRONLY | O_CREAT | O_TRUNC, 0644);
				else if (cmd->type == DRED_OUT)
					fd_out = open(cmd->command[0],
							O_WRONLY | O_CREAT | O_APPEND, 0644);
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

void	exit_child(t_command_list *cmd, int **fd)
{
	struct stat	*stats;
	int			ret;
	int			dir;

	stats = (struct stat *)malloc(sizeof(struct stat));
	dup2(g_all.std_in, STDIN_FILENO);
	dup2(g_all.std_out, STDOUT_FILENO);
	close_fd(fd);
	ret = stat(cmd->command[0], stats);
	dir = S_ISDIR(stats->st_mode);
	free(stats);
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
		ft_exit(cmd, &g_all.exit_status, &g_all.run_status);
}

void	exec(t_command_list *cmd, int **fd)
{
	if (!cmd)
		return ;
	g_all.path = get_path(g_all.envp);
	get_binary(cmd);
//	close_fd(fd);
	if (g_all.binary)
	{
		if (execve(g_all.binary, cmd->command, g_all.envp) == -1)
			exit_child(cmd, fd); //perror
	}
	else
		exit_child(cmd, fd);
}

void	ft_pipe(t_command_list *cmd, int **fd, int i)
{
	pid_t	pid;

	if (!cmd)
		return ;
	if (i < g_all.args->elements - 1)
		pipe(fd[i]);
	pid = fork();
	if (pid == 0)
	{
		handle_redir(cmd);
		if (is_redir(cmd->type))
			exit(1);
		if (i != 0 && g_all.fd_in == 0)
			dup2(fd[i - 1][0], STDIN_FILENO);
		if (next_pipe(cmd) && g_all.fd_out == 0)
			dup2(fd[i][1], STDOUT_FILENO);
		close_fd(fd);
		if (g_all.exec)
			exec(get_cmd(cmd), fd);
		exit(1);
	}
	ft_pipe(next_pipe(cmd), fd, ++i);
	close_fd(fd);
	waitpid(pid, &g_all.exit_status, 0);
	g_all.exit_status = WEXITSTATUS(g_all.exit_status);
}

void	free_darr(void **array)
{
	int	i;

	i = 0;
	while (array && array[i])
		free(array[i++]);
	if (array)
		free(array);
}

int	**create_fd(int num)
{
	int	**fd;
	int	i;
	int	j;

	i = 0;
	j = 0;
	fd = 0;
	if (num <= 1)
		return (0);
	fd = malloc(sizeof(int *) * (num + 1));
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
		fd[i][0] = -1;
		fd[i][1] = -1;
		i++;
	}
	fd[num] = NULL;
	return (fd);
}

void	redir_and_exec(t_command_list *cmd)
{
	struct stat	*stats;
	int			**fd;
	int			i;
	pid_t		pid;
	cmd = get_cmd(cmd);
	pid = -1;
	i = 0;
	fd = 0;
	if (g_all.args->elements > 1)
		fd = create_fd(g_all.args->elements);
	stats = malloc(sizeof(struct stat));
	exec_dredin(cmd);
	if (next_pipe(cmd))
		ft_pipe(cmd, fd, i);
	else
	{
		handle_redir(cmd);
		if (cmd->type == COMMAND)
			pid = fork();
		if (pid == 0)
		{
			if (g_all.exec)
				exec(get_cmd(cmd), fd);
		}
		else
		{	
			close_fd(fd);
			if (is_builtin(cmd->type))
				exec_builtin(cmd);
			if (pid > 0)
				waitpid(pid, &g_all.exit_status, 0);
			g_all.exit_status = WEXITSTATUS(g_all.exit_status);
		}
	}
	close_fd(fd);
	if (!stat(".heredoc", stats))
		unlink(".heredoc");
	free(stats);
	free(g_all.binary);
	g_all.binary = 0;
	if (fd)
		free_darr((void **)fd);
}

void	execute(void)
{
	t_command_list	*cmd;
	tcsetattr(STDIN_FILENO, TCSANOW, &g_all.saved);
	g_all.std_in = dup(STDIN_FILENO);
	g_all.std_out = dup(STDOUT_FILENO);
	cmd = g_all.args->head;
	redir_and_exec(cmd);
	dup2(g_all.std_in, STDIN_FILENO);
	dup2(g_all.std_out, STDOUT_FILENO);
	close(g_all.std_in);
	close(g_all.std_out);
	ft_free();
	tcsetattr(STDIN_FILENO, TCSANOW, &g_all.term);
}

void	loop(void)
{
	char	*line;

	using_history();
	while (g_all.run_status == 0)
	{
		g_all.exec = 1;
		signal(SIGINT, sigint_handler);
		signal(SIGQUIT, SIG_IGN);
		line = readline("minishell> ");
		g_all.path = get_path(g_all.envp);
		if (!line)
		{
			printf("exit\n");
			free_darr((void **)g_all.path);
			tcsetattr(STDIN_FILENO, TCSANOW, &g_all.saved);
			exit(0);
		}
		if (line[0] != '\0')
		{
			add_history(line);
			g_all.args = parser(line, g_all.envp, g_all.exit_status);
			if (g_all.args)
				execute();
		}
		free_darr((void **)g_all.path);
	}
}

char	**save_envp(char **envp)
{
	int		i;
	char	**new_envp;

	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * (i + 1));
	new_envp[i] = 0;
	i = -1;
	while (envp[++i])
		new_envp[i] = ft_strdup(envp[i]);
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
	g_all.run_status = 0;
	rl_event_hook = event;
	g_all.term.c_lflag &= ~(ISIG);
	g_all.term.c_lflag &= ~(ECHOCTL);
	tcsetattr(0, TCSANOW, &g_all.term);
	tgetent(0, term_name);
	loop();
	return (g_all.exit_status);
}

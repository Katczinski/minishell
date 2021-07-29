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

t_command_list	*find_cmd(t_command_list *cmd)
{
	t_command_list	*start;

	start = cmd;
	while (cmd && (cmd->type != COMMAND
			&& !is_builtin(cmd->type) && cmd->type != PIPE))
		cmd = cmd->next;
	if (cmd && (cmd->type == COMMAND || is_builtin(cmd->type)))
		return (cmd);
	return (start);
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
	if (g_all.binary && cmd->type == COMMAND)
	{
		if (execve(g_all.binary, cmd->command, g_all.envp) == -1)
			exit_child(cmd, fd); //perror
		free_darr((void **)fd);
		ft_free();
	}
	else if (is_builtin(cmd->type))
		exec_builtin(cmd);
	else
		exit_child(cmd, fd);
}

void	child(t_command_list *cmd, int **fd, int i)
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
	free_darr((void **)fd);
	ft_free();
	exit(g_all.exit_status);
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
		child(cmd, fd, i);
	ft_pipe(next_pipe(cmd), fd, ++i);
	close_fd(fd);
	waitpid(pid, &g_all.exit_status, 0);
	set_status(g_all.exit_status);
	if (i == g_all.args->elements)
		g_all.last_exit = g_all.exit_status;

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
	struct stat	*stats;
	int			**fd;
	int			i;
	pid_t		pid;

	pid = -1;
	i = 0;
	fd = 0;
	if (g_all.args->elements > 1)
		fd = create_fd(g_all.args->elements);
	stats = malloc(sizeof(struct stat));
	exec_dredin(cmd);
	if (next_pipe(cmd))
	{
		ft_pipe(cmd, fd, i);
		g_all.exit_status = g_all.last_exit;
	}
	else
	{
		handle_redir(cmd);
		if (cmd && cmd->type == COMMAND && g_all.exec)
			pid = fork();
		if (pid == 0)
		{
			if (g_all.exec)
				exec(get_cmd(cmd), fd);
			free_darr((void **)fd);
			ft_free();
		}
		else
		{	
			close_fd(fd);
			if (is_builtin(cmd->type))
				exec_builtin(cmd);
			if (pid > 0)
			{
				waitpid(pid, &g_all.exit_status, 0);
				set_status(g_all.exit_status);
			}
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
	signal(SIGQUIT, &sigquit_handler);
	signal(SIGINT, &sigint_cmd);
	g_all.std_in = dup(STDIN_FILENO);
	g_all.std_out = dup(STDOUT_FILENO);
	cmd = find_cmd(g_all.args->head);
	if (!cmd)
	{
		printf("minishell: : command not found\n");
		g_all.exit_status = 127;
	}
	else
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
		{
			add_history(line);
			g_all.args = parser(line, g_all.envp, g_all.exit_status);
			if (g_all.args)
				execute();
			else
			{
				free(line);
				g_all.exit_status = 2;
			}
		}
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

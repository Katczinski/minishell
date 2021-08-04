#include "minishell.h"

extern t_all	g_all;

void	exec_builtin(t_command_list *cmd)
{
	if (cmd->type == FT_ECHO)
		g_all.exit_status = ft_echo(cmd);
	if (cmd->type == FT_PWD)
		g_all.exit_status = ft_pwd(g_all.args);
	if (cmd->type == FT_CD)
		g_all.exit_status = ft_cd(cmd, &g_all.envp, g_all.args);
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
			exit_child(cmd, fd);
		free_darr((void **)fd);
		ft_free();
	}
	else if (is_builtin(cmd->type))
		exec_builtin(cmd);
	else
		exit_child(cmd, fd);
}

void	exec_single_cmd(t_command_list *cmd, pid_t pid, int **fd)
{
	if (pid == 0)
	{
		close(g_all.std_in);
		close(g_all.std_out);
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

void	child(t_command_list *cmd, int **fd, int i)
{
	cmd = find_cmd(cmd);
	close(g_all.std_in);
	close(g_all.std_out);
	handle_redir(cmd);
	if (is_redir(cmd->type))
		exit(1);
	if (i != 0 && g_all.fd_in == 0)
		dup2(fd[i - 1][0], STDIN_FILENO);
	if (next_pipe(cmd) && g_all.fd_out == 0)
		dup2(fd[i][1], STDOUT_FILENO);
	close_fd(fd);
	if (g_all.exec)
		exec(cmd, fd);
	free_darr((void **)fd);
	ft_free();
	exit(g_all.exit_status);
}

int	ft_pipe(t_command_list *cmd, int **fd, int i)
{
	pid_t	pid;

	if (!cmd)
		return (0);
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
	return (g_all.last_exit);
}

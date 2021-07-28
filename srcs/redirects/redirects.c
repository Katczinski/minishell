#include "minishell.h"

extern t_all	g_all;

int	is_redir(int type)
{
	if (type == RED_IN || type == DRED_IN
		|| type == RED_OUT || type == DRED_OUT)
		return (1);
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
					free(line);
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

void	open_fd_in(t_command_list *cmd)
{			
	if (g_all.fd_in)
		close(g_all.fd_in);
	if (cmd->type == RED_IN)
		g_all.fd_in = open(cmd->command[0], O_RDONLY, 0644);
	else if (cmd->type == DRED_IN)
		g_all.fd_in = open(".heredoc", O_RDONLY, 0644);
}

void	open_fd_out(t_command_list *cmd)
{
	if (g_all.fd_out)
		close(g_all.fd_out);
	if (cmd->type == RED_OUT)
		g_all.fd_out = open(cmd->command[0],
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (cmd->type == DRED_OUT)
		g_all.fd_out = open(cmd->command[0],
				O_WRONLY | O_CREAT | O_APPEND, 0644);
}

void	handle_redir(t_command_list *cmd)
{
	while (cmd->prev && cmd->prev->type != PIPE)
		cmd = cmd->prev;
	while (cmd && cmd->type != PIPE)
	{
		if (is_redir(cmd->type))
		{
			if (cmd->type == RED_IN || cmd->type == DRED_IN)
				open_fd_in(cmd);
			else
				open_fd_out(cmd);
			if (g_all.fd_in == -1 || g_all.fd_out == -1)
			{
				printf("minishell: %s: No such file or directory\n",
					cmd->command[0]);
				g_all.exit_status = 1;
				g_all.exec = 0;
				break ;
			}
		}
		cmd = cmd->next;
	}
	if (g_all.fd_in > 0)
		dup2(g_all.fd_in, STDIN_FILENO);
	if (g_all.fd_out > 0)
		dup2(g_all.fd_out, STDOUT_FILENO);
}

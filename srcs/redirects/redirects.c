#include "minishell.h"

extern t_all	g_all;

void	exec_dredin(t_command_list *cmd)
{
	int		fd;
	int		read;

	fd = 0;
	read = 1;
	while (cmd)
	{
		if (fd)
			close(fd);
		if (cmd->type == DRED_IN)
		{
			fd = open(".heredoc", O_CREAT | O_WRONLY | O_TRUNC, 0644);
			while (read)
				read = heredoc_reader(fd, cmd);
		}		
		cmd = cmd->next;
		read = 1;
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
	while (cmd && cmd->prev && cmd->prev->type != PIPE)
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
	ft_dup2();
}

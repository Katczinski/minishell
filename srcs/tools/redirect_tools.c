#include "minishell.h"

extern t_all	g_all;

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

int	heredoc_reader(int fd, t_command_list *cmd)
{
	char	*line;

	line = readline("> ");
	if (!line)
		return (0);
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
		return (0);
	}
	return (1);
}

void	ft_dup2(void)
{
	if (g_all.fd_in > 0)
		dup2(g_all.fd_in, STDIN_FILENO);
	if (g_all.fd_out > 0)
		dup2(g_all.fd_out, STDOUT_FILENO);
}

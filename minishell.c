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

void	redir_and_exec(t_command_list *cmd)
{
	int			**fd;
	int			i;
	pid_t		pid;

	pid = -1;
	i = 0;
	fd = 0;
	if (g_all.args->elements > 1)
		fd = create_fd(g_all.args->elements);
	exec_dredin(cmd);
	if (next_pipe(cmd))
		g_all.exit_status = ft_pipe(cmd, fd, i);
	else
	{
		handle_redir(cmd);
		if (cmd && cmd->type == COMMAND && g_all.exec)
			pid = fork();
		exec_single_cmd(cmd, pid, fd);
	}
	close_fd(fd);
	free(g_all.binary);
	g_all.binary = 0;
	if (fd)
		free_darr((void **)fd);
}

void	execute(void)
{
	t_command_list	*cmd;
	struct stat		*stats;

	stats = malloc(sizeof(struct stat));
	set_term();
	cmd = find_cmd(g_all.args->head);
	if (!cmd)
	{
		printf("minishell: : command not found\n");
		g_all.exit_status = 127;
	}
	else
		redir_and_exec(cmd);
	if (!stat(".heredoc", stats))
		unlink(".heredoc");
	free(stats);
	dup2(g_all.std_in, STDIN_FILENO);
	dup2(g_all.std_out, STDOUT_FILENO);
	close(g_all.std_in);
	close(g_all.std_out);
	ft_free();
	tcsetattr(STDIN_FILENO, TCSANOW, &g_all.term);
}

void	parse_and_execute(char **line)
{
	add_history(*line);
	g_all.args = parser(*line, g_all.envp, g_all.exit_status);
	if (g_all.args)
		execute();
	else
	{
		free(*line);
		g_all.exit_status = 2;
	}
}

void	loop(void)
{
	char	*line;

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
			parse_and_execute(&line);
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

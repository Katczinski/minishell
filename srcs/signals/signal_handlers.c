#include "minishell.h"

extern t_all	g_all;

void	set_status(int status)
{
	if (WIFEXITED(status))
		g_all.exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		g_all.exit_status = 128 + WTERMSIG(status);
}

void	sigint_handler(int signo)
{
	if (signo && signo == SIGINT)
	{
		g_all.exit_status = 1;
		rl_redisplay();
		rl_replace_line("", 0);
		rl_done = 1;
	}
}

void	sigint_cmd(int signo)
{
	if (signo && signo == SIGINT)
		g_all.exit_status = 130;
}

void	sigquit_handler(int signo)
{
	if (signo && signo == SIGQUIT)
		printf("Quit: %d\n", signo);
}

int	event(void)
{
	return (0);
}

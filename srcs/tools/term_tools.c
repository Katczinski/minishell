#include "minishell.h"

extern t_all	g_all;

void	set_term(void)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &g_all.saved);
	signal(SIGQUIT, &sigquit_handler);
	signal(SIGINT, &sigint_cmd);
	g_all.std_in = dup(STDIN_FILENO);
	g_all.std_out = dup(STDOUT_FILENO);
}

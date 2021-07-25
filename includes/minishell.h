#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include "parser.h"
# include "builtins.h"
# include <unistd.h>
# include <term.h>
# include <termios.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <string.h>

typedef struct	s_all
{
	struct termios	term;
	struct termios	saved;
	int				status;
	int				exit_status;
	char			**path;
	char			**envp;
	char			*binary;
	int				fd_in;
	int				fd_out;
	int				exec;
	t_info			*args;
}					t_all;


int					ft_putchar(int c);

#endif

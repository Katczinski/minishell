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
	int				run_status;
	int				exit_status;
	char			**path;
	char			**envp;
	char			*binary;
	int				fd_in;
	int				fd_out;
	int				std_in;
	int				std_out;
	int				exec;
	t_info			*args;
}					t_all;


int					ft_putchar(int c);
int					is_redir(int type);
int					heredoc_reader(int fd, t_command_list *cmd);
void				ft_dup2(void);
void				exec_dredin(t_command_list *cmd);
void				handle_redir(t_command_list *cmd);
char				*subst_value(char *line);
#endif

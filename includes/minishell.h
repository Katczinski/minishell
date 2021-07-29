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
	int				last_exit;
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
int					is_all_whitespaces(char *str);
int					is_builtin(int type);
int					event(void);
int					heredoc_reader(int fd, t_command_list *cmd);
int					ft_pipe(t_command_list *cmd, int **fd, int i);
void				set_term(void);
void				exec_builtin(t_command_list *cmd);
void				exec(t_command_list *cmd, int **fd);
void				exec_single_cmd(t_command_list *cmd, pid_t pid, int **fd);
void				child(t_command_list *cmd, int **fd, int i);
void				get_binary(t_command_list *cmd);
void				sigint_handler(int signo);
void				sigint_cmd(int signo);
void				sigquit_handler(int signo);
void				ft_dup2(void);
void				exec_dredin(t_command_list *cmd);
void				handle_redir(t_command_list *cmd);
void				free_darr(void **array);
void				exit_child(t_command_list *cmd, int **fd);
void				ft_free(void);
void				close_fd(int **fd);
void				set_status(int status);
char				**get_path(char **envp);
char				**save_envp(char **envp);
char				*get_shlvl(char *envp);
char				*subst_value(char *line);
t_command_list		*find_cmd(t_command_list *cmd);
t_command_list		*get_cmd(t_command_list *cmd);
t_command_list		*next_pipe(t_command_list *cmd);
#endif

#ifndef BUILTINS_H
# define BUILTINS_H
// # ifndef BUFFER_SIZE
// #  define BUFFER_SIZE 4096
// # endif
// # include <stdio.h>
// # include <unistd.h>
// # include <stdlib.h>
// # include <fcntl.h>
// # include <readline/readline.h>
// # include <readline/history.h>
// # include <errno.h>
# include <dirent.h>
# include "parser.h"
// # include "minishell.h"

int		ft_echo(t_command_list *list);
int 	ft_pwd(t_info *info);
int		ft_cd(t_command_list *list, char **envp, t_info *info);
char	**ft_export(t_command_list *list, char **envp, t_info *info);
char	**ft_unset(t_command_list *list, char **envp, t_info *info);
int		find_envp(char *name, char **envp);
int		ft_env(char **envp);
void		ft_exit(t_command_list *list, int *exit_status, int *status);

#endif

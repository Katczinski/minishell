#ifndef BUILTINS_H
# define BUILTINS_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif
// # include <stdio.h>
// # include <unistd.h>
// # include <stdlib.h>
// # include <fcntl.h>
// # include <readline/readline.h>
// # include <readline/history.h>
// # include <errno.h>
# include <dirent.h>
# include "minishell.h"

//ECHO
int	ft_echo(t_command_list *list);
int ft_pwd(t_info *info);


#endif

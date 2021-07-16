#ifndef SHELLY_H
# define SHELLY_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <errno.h>
# include "libft.h"

# define COMMAND 111
# define BUILT_IN 112
# define RED_IN 113
# define DRED_IN 114
# define RED_OUT 115
# define DRED_OUT 116

typedef struct s_command_list
{
	int					type;
	char    			**command;
	// char				*file_name;
	int					lines;
	struct	s_command_list	*next;
}   				t_command_list;

typedef struct s_info
{
    t_command_list		*head;
    t_command_list		*tail;
    int				elements;
}  				t_info;



//Utils
int				get_next_line(int fd, char **line);
int				print_error(char *msg, t_info *info);
int				skip_whitespaces(int i, char *line);
char	    			**ft_split_modified(char const *s);

//Parsing
t_info  		       	 *init_struct(void);
t_info 				*parser(char *line, char **envp);
t_command_list			*init_element(t_info *info);
void				add_element(t_command_list *element, t_info *info);
char				**add_line_to_cmd(char *line, t_command_list *cmd, t_info *info);


#endif

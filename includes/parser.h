#ifndef PARSER_H
# define PARSER_H
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

# define COMMAND 105
# define FT_ECHO 106
# define FT_CD 107
# define FT_PWD 108
# define FT_EXPORT 109
# define FT_UNSET 110
# define FT_ENV 111
# define FT_EXIT 112
# define RED_IN 113
# define DRED_IN 114
# define RED_OUT 115
# define DRED_OUT 116
# define PIPE 117

typedef struct s_command_list
{
	int						type;
	int						quoted;
	char					**command;
	int						lines;
	struct s_command_list	*next;
	struct s_command_list	*prev;
}				t_command_list;

typedef struct s_info
{
	t_command_list		*head;
	t_command_list		*tail;
	int					status;
	int					elements;
	int					quotes;
	int					dquotes;
	int					redirects;
}				t_info;

typedef struct s_ft_env
{
	char	*prev_str;
	char	*curr_str;
	char	*next_str;
	char	*tmp;
	char	*key;
	int		start;
	int		j;
}				t_ft_env;

typedef struct s_ft_quote
{
	char	*prev_str;
	char	*curr_str;
	char	*next_str;
	char	*tmp;
	int		frst_quote;
}				t_ft_quote;

typedef struct s_ft_dquote
{
	char	*prev_str;
	char	*curr_str;
	char	*next_str;
	char	*tmp;
	int		frst_quote;
}				t_ft_dquote;

int				print_error(char *msg, t_info *info, int mode);
int				skip_whitespaces(int i, char *line);
char			*treat_env(char *line, int *i, char **envp, t_info *info);
char			*treat_quote(char *line, int *i, t_info *info);
char			*treat_dquote(char *line, int *i, char **envp, t_info *info);
char			*treat_pipe(char *line, int *i, t_info *info);
int				copy_prev_line(t_info *info, int *i, char *line);
char			*treat_redirect(char *line, int *i, char **envp, t_info *info);
char			*treat_space(char *line, int *i, char **envp, t_info *info);


// void			ft_free_lines(char *str1, char *str2, char *str3, char *str4);
t_info			*init_struct(int status);
t_info			*parser(char *line, char **envp, int status);
t_command_list	*init_element(t_info *info);
void			add_element(t_command_list *element, t_info *info);
char			**add_line_to_cmd(char *line,
					t_command_list *cmd, t_info *info);
int				line_check(char *line, t_info *info);

#endif

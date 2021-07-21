SRCS = minishell.c\
	srcs/parser/create_list.c\
	srcs/parser/init.c\
	srcs/parser/parser.c\
	srcs/builtins/ft_echo.c\
	srcs/builtins/ft_cd.c\
	srcs/builtins/ft_pwd.c\
	srcs/builtins/ft_export.c\
	srcs/builtins/ft_unset.c\
	srcs/builtins/ft_env.c\
	srcs/builtins/ft_exit.c\
	srcs/utils/ft_split_modified.c\
	srcs/utils/get_next_line.c\
	srcs/utils/print_error.c\
	srcs/utils/skip_whitespaces.c\

OBJS = $(patsubst %.c,%.o,$(SRCS))

NAME = minishell

INC = -Iincludes -Ilibft -I /usr/local/Cellar/readline/8.1/include

LIBS = -lreadline -ltermcap -Llibft -lft -L /usr/local/Cellar/readline/8.1/lib

FLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS)
		make -C ./libft
		gcc $(FLAGS) -g $(OBJS) -o $(NAME) $(INC) $(LIBS) 

%.o: %.c
		gcc $(FLAGS) -g $(INC) -c $< -o $@

clean:
		make -C ./libft clean
		rm -rf $(OBJS)

fclean: clean
		make -C ./libft fclean
		rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re

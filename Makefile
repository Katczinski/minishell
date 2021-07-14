SRCS = minishell.c\
	srcs/parser/create_list.c\
	srcs/parser/init.c\
	srcs/parser/parser.c\
	srcs/utils/ft_split_modified.c\
	srcs/utils/get_next_line.c\
	srcs/utils/print_error.c\
	srcs/utils/skip_whitespaces.c\

OBJS = $(patsubst %.c,%.o,$(SRCS))

NAME = minishell

INC = -Iincludes -Ilibft

FLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS)
		make -C ./libft
		gcc $(FLAGS) -g $(OBJS) -o $(NAME) $(INC) -lreadline -ltermcap -Llibft -lft

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

SRCS = minishell.c\
	srcs/parser/create_list.c\
	srcs/parser/init.c\
	srcs/parser/parser.c\
	srcs/parser/line_checks.c\
	srcs/parser/treat_env.c\
	srcs/parser/treat_quote.c\
	srcs/parser/treat_dquote.c\
	srcs/parser/treat_pipe.c\
	srcs/parser/treat_redirect.c\
	srcs/parser/treat_space.c\
	srcs/builtins/ft_echo.c\
	srcs/builtins/ft_cd.c\
	srcs/builtins/ft_pwd.c\
	srcs/builtins/ft_export.c\
	srcs/builtins/ft_unset.c\
	srcs/builtins/ft_env.c\
	srcs/builtins/ft_exit.c\
	srcs/utils/print_error.c\
	srcs/utils/backward_skip_whitespaces.c\
	srcs/utils/ft_export_utils_for_print.c\
	srcs/utils/ft_export_utils_for_add.c\
	srcs/utils/treat_env_utils.c\
	srcs/redirects/redirects.c\
	srcs/free/free_and_exit.c\
	srcs/tools/redirect_tools.c\
	srcs/tools/is_tools.c\
	srcs/tools/term_tools.c\
	srcs/tools/cmd_tools.c\
	srcs/tools/fd_tools.c\
	srcs/signals/signal_handlers.c\
	srcs/env/env_handlers.c\
	srcs/exec/exec.c\

OBJS = $(patsubst %.c,%.o,$(SRCS))

NAME = minishell

UBU = /usr/local
MAC = /Users/abirthda/.brew
USER = $(UBU)

INC = -Iincludes -Ilibft -I $(USER)/Cellar/readline/8.1/include

LIBS = -lreadline -ltermcap -Llibft -lft -L $(USER)/Cellar/readline/8.1/lib

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

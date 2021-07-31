#include "builtins.h"

static int	has_only_digits(char *cmd)
{
	int	i;

	i = 0;
	while (cmd[i] && ft_isdigit(cmd[i]))
		i++;
	if (!cmd[i])
		return (1);
	return (0);
}

static int	find_pipe(t_command_list *list)
{
	t_command_list	*tmp;

	tmp = list;
	while (tmp)
	{
		if (tmp->type == PIPE)
			return (1);
		tmp = tmp->prev;
	}
	tmp = list;
	while (tmp)
	{
		if (tmp->type == PIPE)
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

static void	change_status(int *exit_status, int *status, int mode)
{
	if (!mode)
	{
		printf("minishell: exit: too many arguments\n");
		*exit_status = 1;
		*status = 0;
	}
	else
	{
		printf("minishell: exit: numeric argument required\n");
		*exit_status = 255;
	}
}

void	ft_exit(t_command_list *list, int *exit_status, int *status)
{
	int	value;

	if (list->command[1])
		value = ft_atoi(list->command[1]);
	if (!find_pipe(list))
	{
		printf("exit\n");
		*status = 1;
	}
	if (!list->command[1])
		*exit_status = 0;
	else if (list->command[1] && list->command[2]
		&& has_only_digits(list->command[1]))
		change_status(exit_status, status, 0);
	else if (list->command[1] && !has_only_digits(list->command[1]))
		change_status(exit_status, status, 1);
	else if (list->command[1] && !list->command[2]
		&& has_only_digits(list->command[1]))
	{
		if (value < 0 || value > 255)
			*exit_status = value & 0377;
		else
			*exit_status = value;
	}
}

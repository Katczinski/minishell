#include "parser.h"

static void	ft_free_struct(t_info *info)
{
	int				i;
	t_command_list	*tmp;

	tmp = info->head;
	i = -1;
	if (tmp)
	{
		while (tmp)
		{
			if (tmp->command)
			{
				while (tmp->command[++i])
					free(tmp->command[i]);
				free(tmp->command);
			}
			tmp = tmp->next;
			if (tmp && tmp->prev)
				free(tmp->prev);
		}
		if (tmp && tmp->command)
			free(tmp->command);
	}
	free(info);
}

int	print_error(char *msg, t_info *info)
{
	if (msg)
		printf("%s\n", msg);
	if (info)
		ft_free_struct(info);
	return (1);
}

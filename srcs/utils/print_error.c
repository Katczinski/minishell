#include "parser.h"

static void	ft_free_struct(t_info *info)
{
	int	i;

	i = -1;
	if (info->head)
	{
		while (info->head)
		{
			if (info->head->command)
			{
				while (info->head->command[++i])
					free(info->head->command[i]);
				free(info->head->command);
			}
			info->head = info->head->next;
			free(info->head->prev);
		}
		free(info->head->command);
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

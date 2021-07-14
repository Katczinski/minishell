#include "parser.h"

t_command_list	*init_element(t_info *info)
{
	t_command_list *element;

	element = malloc(sizeof(t_command_list));
	if (!element)
	{
		print_error(strerror(errno), info);
		return (0);
	}
	element->type = 0;
	element->command = 0;
	element->lines = 0;
	element->redirect = 0;
	element->file_name = 0;
	element->next = 0;
	return (element);
}

void	add_element(t_command_list *element, t_info *info)
{
	if (!info->head)
	{
		info->head = element;
		info->tail = element;
	}
	else
	{
		info->tail->next = element;
		info->tail = element;
	}
}

char **add_line_to_arr(char *line, t_command_list *cmd, t_info *info)
{
	char	**arr;
	int		j;

	arr = malloc(sizeof(char *) * (cmd->lines + 1));
	if (!arr)
	{
		print_error(strerror(errno), info);
		return (0);
	}
	j = -1;
	if(cmd->command)
	{
		while (cmd->command[++j])
		{
			arr[j] = malloc(sizeof(char) * (ft_strlen(cmd->command[j]) + 1));
			arr[j] = ft_memcpy(arr[j], cmd->command[j], ft_strlen(cmd->command[j]));
			free(cmd->command[j]);
		}
		free(cmd->command);
	}
	arr[cmd->lines - 1] = malloc(sizeof(char) * (ft_strlen(line) + 1));
	arr[cmd->lines - 1] = ft_memcpy(arr[cmd->lines - 1], line, ft_strlen(line));
	arr[cmd->lines] = 0;
	return (arr);
}

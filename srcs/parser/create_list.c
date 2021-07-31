#include "parser.h"

t_command_list	*init_element(t_info *info)
{
	t_command_list	*element;

	element = malloc(sizeof(t_command_list));
	if (!element)
	{
		print_error(strerror(errno), info, 1);
		return (0);
	}
	element->type = 0;
	element->quoted = 0;
	element->command = 0;
	element->lines = 0;
	element->next = 0;
	element->prev = 0;
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
		element->prev = info->tail;
		info->tail = element;
	}
}

char	**add_line_to_cmd(char *line, t_command_list *cmd, t_info *info)
{
	char	**arr;
	int		j;

	arr = malloc(sizeof(char *) * (cmd->lines + 1));
	if (!arr)
	{
		print_error(strerror(errno), info, 1);
		return (0);
	}
	j = -1;
	if (cmd->command)
	{
		while (cmd->command[++j])
		{
			arr[j] = ft_strdup(cmd->command[j]);
			free(cmd->command[j]);
		}
		free(cmd->command);
	}
	arr[cmd->lines - 1] = ft_strdup(line);
	arr[cmd->lines] = 0;
	free(line);
	return (arr);
}

void	throw_args_to_cmd(t_command_list *list, t_info *info)
{
	t_command_list	*tmp;
	int				i;

	tmp = list;
	while (tmp)
	{
		if (tmp->type == COMMAND || tmp->type == FT_ECHO || tmp->type == FT_CD
			|| tmp->type == FT_PWD || tmp->type == FT_EXPORT
			|| tmp->type == FT_UNSET || tmp->type == FT_EXIT
			|| tmp->type == FT_ENV)
			break ;
		tmp = tmp->prev;
	}
	i = 1;
	if (tmp && tmp->command)
	{
		while (i < list->lines)
		{
			tmp->lines++;
			tmp->command = add_line_to_cmd(list->command[i], tmp, info);
			i++;
		}
		list->lines = 1;
		list->command[1] = 0;
	}
}

#include "builtins.h"

int	ft_echo(t_command_list *list)
{
	int	i;
	int new_line;

	i = 0;
	new_line = 0;
	while (list->command[++i])
	{
		if ((i > 2 && !ft_strcmp(list->command[1], "-n")) || (i > 1 && ft_strcmp(list->command[1], "-n")))
			printf(" ");
		if (i == 1 && !ft_strcmp(list->command[i], "-n"))
			new_line++;
		else
			printf("%s", list->command[i]);
	}
	if (!new_line)
		printf("\n");
	return (0);
}
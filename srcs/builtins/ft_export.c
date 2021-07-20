#include "builtins.h"

char **copy_envp(char **envp)
{
	char **arr;
	int i;

	i = 0;
	while (envp[i])
		i++;
	arr = malloc(sizeof(char *) * (i + 1));
	arr[i] = 0;
	i = -1;
	while (envp[++i])
		arr[i] = strdup(envp[i]);
	return (arr);
}

void	sort_arr(char **arr)
{
	int i;
	int j;
	char *tmp;

	i = 0;
	while (arr[i])
	{
		j = i + 1;
		while (arr[j])
		{
			if (ft_strcmp(arr[i], arr[j]) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

void	print_envp(char **envp)
{
	char **arr;
	int i;
	int j;

	arr = copy_envp(envp);
	sort_arr(arr);
	i = -1;
	while (arr[++i])
	{
		j = -1;
		printf("declare -x ");
		while(arr[i][++j] != '=')
			printf("%c", arr[i][j]);
		if (arr[i][j] == '=')
			printf("=\"");
		while(arr[i][++j])
			printf("%c", arr[i][j]);
		printf("\"\n");
	}
	i = -1;
	while (arr[++i])
		free(arr[i]);
	free(arr);
}

int ft_export(t_command_list *list, char **envp, t_info *info)
{
    if (list->lines == 1)
    {
        print_envp(envp);
    }
	info->elements++;
	return (0);
}

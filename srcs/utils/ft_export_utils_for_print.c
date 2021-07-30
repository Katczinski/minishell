#include "builtins.h"

char	**copy_envp(char **envp, t_info *info)
{
	char	**arr;
	int		i;

	i = 0;
	while (envp[i])
		i++;
	arr = malloc(sizeof(char *) * (i + 1));
	if (!arr)
	{
		print_error(strerror(errno), info, 0);
		return (0);
	}
	arr[i] = 0;
	i = -1;
	while (envp[++i])
	{
		arr[i] = strdup(envp[i]);
		if (!arr[i])
		{
			print_error(strerror(errno), info, 0);
			return (0);
		}
	}
	return (arr);
}

void	sort_arr(char **arr)
{
	int		i;
	int		j;
	char	*tmp;

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

void	print_env_lines(char **arr, int i, int j)
{
	while (arr[++i])
	{
		j = 0;
		printf("declare -x ");
		while (arr[i][j] && arr[i][j] != '=')
		{
			printf("%c", arr[i][j]);
			j++;
		}
		if (arr[i][j] == '=')
		{
			printf("=\"");
			j++;
		}
		while (arr[i][j])
		{
			printf("%c", arr[i][j]);
			j++;
		}
		if (value_found(arr[i]))
			printf("\"\n");
		else
			printf("\n");
	}
}

void	free_arr(char **arr)
{
	int	i;

	i = -1;
	if (arr)
	{
		while (arr[++i])
			free(arr[i]);
		free(arr);
	}
}

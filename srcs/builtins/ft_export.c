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
		print_error(strerror(errno), info);
		return (0);
	}
	arr[i] = 0;
	i = -1;
	while (envp[++i])
	{
		arr[i] = strdup(envp[i]);
		if (!arr[i])
		{
			print_error(strerror(errno), info);
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

int	value_found(char *cmd)
{
	int	i;

	i = 0;
	while(cmd[i] && cmd[i] != '=')
		i++;
	if (cmd[i] == '=')
		return (1);
	return (0);
}

int	print_envp(char **envp, t_info *info)
{
	char	**arr;
	int		i;
	int		j;

	arr = copy_envp(envp, info);
	if (!arr)
		return (1);
	sort_arr(arr);
	i = -1;
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
	free_arr(arr);
	return (0);
}

int	check_envp(char **cmd, t_info *info)
{
	int i;
	int j;

	i = 0;
	while (cmd[++i])
	{
		j = 0;
		if (!ft_isalpha(cmd[i][j]) && cmd[i][j] != '_')
			return (print_error("minishell: export: not a valid identifier", info));
		while (cmd[i][j] && cmd[i][j] != '=' && (ft_isalnum(cmd[i][j]) || cmd[i][j] == '_'))
			j++;
		if (cmd[i][j] && cmd[i][j] != '=')
			return (print_error("minishell: export: not a valid identifier", info));
	}
	return (0);
}

char *save_name(char *envp_str, t_info *info)
{
	int		i;
	char	*name;

	i = 0;
	name = 0;
	while (envp_str[i] && envp_str[i] != '=')
		i++;
	name = malloc(sizeof(char) * i + 1);
	if (!name)
	{
		print_error(strerror(errno), info);
		return (0);
	}
	name = ft_memcpy(name, envp_str, i);
	return (name);
}

int	change_value(char *env_name, char *cmd, char **envp, t_info *info)
{
	int	i;

	i = 0;
	if (env_name && cmd)
	{
		while (envp[i] && ft_strncmp(envp[i], env_name, ft_strlen(env_name)))
			i++;
		// if (envp[i][ft_strlen(env_name)] == '=')
		// }
		if (value_found(cmd))
		{
			free(envp[i]);
			envp[i] = ft_strdup(cmd);
		}
		if (!envp[i])
			return (print_error(strerror(errno), info));
		// }
	}
	return (0);
}

char **create_envp(char *cmd, char **envp, t_info *info)
{
	char	**new_envp;
	int		i;

	new_envp = 0;
	i = 0;
	if (cmd && envp)
	{
		while (envp[i])
			i++;
		new_envp = malloc(sizeof(char *) * (i + 2));
		if (!new_envp)
		{
			print_error(strerror(errno), info);
			return (0);
		}
		new_envp[i + 1] = 0;
		i = -1;
		while (envp[++i])
		{
			new_envp[i] = ft_strdup(envp[i]);
			if (!new_envp[i])
			{
				print_error(strerror(errno), info);
				return (0);
			}
		}
		new_envp[i] = ft_strdup(cmd);
		if (!new_envp[i])
		{
			print_error(strerror(errno), info);
			return (0);
		}
	}
	free_arr(envp);
	return (new_envp);
}


int	find_envp(char *name, char **envp)
{
	int i;
	int len;

	
	i = -1;
	while (envp[++i])
	{
		len = 0;
		while (envp[i][len] && envp[i][len] != '=')
			len++;
		if (!ft_strncmp(envp[i], name, len))
			break ;
	}
	if (!envp[i])
		return (0);
	return (1);
}

char **add_envp(char **cmd, char **envp, t_info *info)
{
	int i;
	char *env_name;

	i = 0;
	env_name = 0;
	while (cmd[++i])
	{
		env_name = save_name(cmd[i], info);
		if (!env_name)
		{
			print_error(strerror(errno), info);
			return (0);
		}
		if (find_envp(env_name, envp))
		{
			if (change_value(env_name, cmd[i], envp, info))
				return (0);
		}
		else
		// {
		// 	if (value_found(cmd[i]))
				envp = create_envp(cmd[i], envp, info);
		// }
		free(env_name);
	}
	return (envp);
}

int ft_export(t_command_list *list, char ***envp, t_info *info)
{
	int	ret;

	ret = 0;
	if (list->lines == 1)
		ret = print_envp(*envp, info);
	else
	{
		if (check_envp(list->command, info))
			ret = 1;
		*envp = add_envp(list->command, *envp, info);
	}
	if (!(*envp))
		ret = 1;
	return (ret);
}

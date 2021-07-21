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
	int i;
	char *name;

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
	// printf("name: %s\n", name);
	return (name);
}

// char *save_value(char *envp_str, t_info *info)
// {
// 	int i;
// 	char *value;

// 	i = 0;
// 	value = 0;
// 	while (envp_str[i] && envp_str[i] != '=')
// 		i++;
// 	if (envp_str[i] != '=')
// 		return (0);
// 	value = ft_strdup(envp_str + i + 1);
// 	if (!value)
// 	{
// 		print_error(strerror(errno), info);
// 		return (0);
// 	}
// 	// printf("value: %s\n", value);
// 	return (value);
// }

int	change_value(char *env_name, char *cmd, char **envp, t_info *info)
{
	// char *new_value;
	// char *tmp;
	int i;

	// new_value = 0;
	// tmp = 0;
	i = 0;
	if (env_name && cmd)
	{

		while(envp[i] && ft_strncmp(envp[i], env_name, ft_strlen(env_name)))
			i++;
		if (envp[i][ft_strlen(env_name)] == '=')
		{
			// tmp = ft_strjoin(env_name, "=");
			// new_value = ft_strjoin(tmp, env_value);	
			// envp[i] = new_value;
			free(envp[i]);
			envp[i] = ft_strdup(cmd);
			if (!envp[i])
				return(print_error(strerror(errno), info));
			// free(tmp);
		}
	}
	return (0);
}

int	value_found(char *cmd)
{
	int i;

	i = 0;
	while(cmd[i] && cmd[i] != '=')
		i++;
	if (cmd[i] == '=')
		return (1);
	return (0);

}

char **create_envp(char *cmd, char **envp, t_info *info)
{
	char **new_envp;
	int i;

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
			new_envp[i] = ft_strdup(envp[i]);
		new_envp[i] = ft_strdup(cmd);
		// if (!new_envp)
		// {
			// print_error(strerror(errno), info);
			// return (0);
		// }
	}
	i = -1;
	if (new_envp)
	{
		while (envp[++i])
			free(envp[i]);
		free(envp);
	}
	return (new_envp);
}

int	find_envp(char *name, char **envp)
{
	int i;
	int len;

	len = ft_strlen(name);
	i = 0;
	while (envp[i] && ft_strncmp(envp[i], name, len))
		i++;
	if (!envp[i] || envp[i][len] != '=')
		return (0);
	return (1);
}

char **add_envp(char **cmd, char **envp, t_info *info)
{
	int i;
	char *env_name;
	// char *new_value;
	// char **new_envp;

	i = 0;
	env_name = 0;
	// new_value = 0;
	while (cmd[++i])
	{
		env_name = save_name(cmd[i], info);
		// new_value = save_value(cmd[i], info);
		if (find_envp(env_name, envp))
			change_value(env_name, cmd[i], envp, info);
		else
		{
			if (value_found(cmd[i]))
				envp = create_envp(cmd[i], envp, info);
		}
		free(env_name);
		// free(new_value);
	}
	//
	
	//
	return (envp);
}

char **ft_export(t_command_list *list, char **envp, t_info *info)
{
    if (list->lines == 1)
        print_envp(envp);
	else
	{
		if (check_envp(list->command, info))
			return (envp);
		envp = add_envp(list->command, envp, info);
	}
	// int i = -1;
	// while (envp[++i])
	// 	printf("%s\n", envp[i]);


	return (envp);
}

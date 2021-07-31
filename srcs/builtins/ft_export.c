#include "builtins.h"

int	print_envp(char **envp, t_info *info)
{
	char	**arr;
	int		i;
	int		j;

	arr = copy_envp(envp, info);
	if (!arr)
		return (1);
	i = -1;
	j = 0;
	sort_arr(arr);
	print_env_lines(arr, i, j);
	free_arr(arr);
	return (0);
}

int	check_envp(char *cmd, t_info *info, int *to_add)
{
	int	j;

	j = 0;
	if (!ft_isalpha(cmd[j]) && cmd[j] != '_')
	{
		*to_add = 0;
		return (print_error("minishell: export: not a valid identifier",
				info, 0));
	}
	while (cmd[j] && cmd[j] != '='
		&& (ft_isalnum(cmd[j]) || cmd[j] == '_'))
		j++;
	if (cmd[j] && cmd[j] != '=')
	{
		*to_add = 0;
		return (print_error("minishell: export: not a valid identifier",
				info, 0));
	}
	return (0);
}

char	**copy_line_to_new_envp(char **new_envp,
			char **envp, char *cmd, t_info *info)
{
	char	**arr;
	int		i;

	arr = new_envp;
	i = -1;
	while (envp[++i])
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			print_error(strerror(errno), info, 0);
			return (0);
		}
	}
	new_envp[i] = ft_strdup(cmd);
	if (!new_envp[i])
	{
		print_error(strerror(errno), info, 0);
		return (0);
	}
	return (arr);
}

char	**add_envp(char *cmd, char **envp, t_info *info)
{
	char	*env_name;

	env_name = 0;
	env_name = save_name(cmd, info);
	if (!env_name)
	{
		print_error(strerror(errno), info, 0);
		return (0);
	}
	if (find_envp(env_name, envp))
	{
		if (change_value(env_name, cmd, envp, info))
			return (0);
	}
	else
		envp = create_envp(cmd, envp, info);
	free(env_name);
	return (envp);
}

int	ft_export(t_command_list *list, char ***envp, t_info *info)
{
	int	ret;
	int	i;
	int	to_add;

	ret = 0;
	i = 0;
	if (list->lines == 1)
		ret = print_envp(*envp, info);
	else
	{
		while (list->command[++i])
		{
			to_add = 1;
			if (check_envp(list->command[i], info, &to_add))
				ret = 1;
			if (to_add)
				*envp = add_envp(list->command[i], *envp, info);
		}
	}
	if (!(*envp))
		ret = 1;
	return (ret);
}

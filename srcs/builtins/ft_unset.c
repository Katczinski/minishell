#include "builtins.h"

int	has_value(char *cmd)
{
	int	i;

	i = -1;
	while (cmd[++i])
		if (cmd[i] == '=')
			return (1);
	return (0);
}

int	find_envp_to_del(char *name, char **envp)
{
	int	i;
	int	len;

	i = -1;
	if (has_value(name))
	{
		while (envp[++i])
			if (!ft_strcmp(envp[i], name))
				break ;
	}
	else
	{
		while (envp[++i])
		{
			len = 0;
			while (envp[i][len] && envp[i][len] != '=')
				len++;
			if (!ft_strncmp(envp[i], name, len))
				break ;
		}
	}
	if (!envp[i])
		return (0);
	return (1);
}

char	**copy_second_part(char **new_envp, char **envp, int i, t_info *info)
{
	int		j;
	char	**arr;

	j = i;
	arr = new_envp;
	while (envp[++i])
	{
		new_envp[j] = strdup(envp[i]);
		if (!new_envp[j])
		{
			print_error(strerror(errno), info, 0);
			return (0);
		}
		j++;
	}
	free_arr(envp);
	return (arr);
}

char	**delete_envp(char *cmd, char **envp, t_info *info)
{
	char	**new_envp;
	int		i;

	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * i);
	if (!new_envp)
	{
		print_error(strerror(errno), info, 0);
		return (0);
	}
	new_envp[i - 1] = 0;
	i = -1;
	while (envp[++i] && ft_strncmp(envp[i], cmd, ft_strlen(cmd)))
	{
		new_envp[i] = strdup(envp[i]);
		if (!new_envp[i])
		{
			print_error(strerror(errno), info, 0);
			return (0);
		}
	}
	new_envp = copy_second_part(new_envp, envp, i, info);
	return (new_envp);
}

int	ft_unset(t_command_list *list, char ***envp, t_info *info)
{
	int	i;

	i = 0;
	while (list->command[++i])
	{
		if (find_envp_to_del(list->command[i], *envp))
			*envp = delete_envp(list->command[i], *envp, info);
		else
			return (1);
	}
	return (0);
}

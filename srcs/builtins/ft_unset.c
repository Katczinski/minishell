#include "builtins.h"

char **delete_envp(char *cmd, char **envp, t_info *info)
{
    char **new_envp;
	int i;
	int j;
	int len;

    i = 0;
	len = ft_strlen(cmd);
    while (envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * i);
	if (!new_envp)
	{
		print_error(strerror(errno), info);
		return (0);
	}
	new_envp[i - 1] = 0;
	i = 0;
	while (envp[i] && ft_strncmp(envp[i], cmd, len))
	{
		new_envp[i] = strdup(envp[i]);
		if (!new_envp[i])
		{
			print_error(strerror(errno), info);
			return (0);
		}
		i++;
	}
	j = i;
	while (envp[++i])
	{
		new_envp[j] = strdup(envp[i]);
		if (!new_envp[j])
		{
			print_error(strerror(errno), info);
			return (0);
		}
		j++;
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

char **ft_unset(t_command_list *list, char **envp, t_info *info)
{
	int i;
	// char **new_envp;

	i = 0;
    // new_envp = 0;
	while (list->command[++i])
	{
		if (find_envp(list->command[i], envp))
			envp = delete_envp(list->command[i], envp, info);
	}
	// if (!new_envp)
	// 	return (envp);
	i = -1;
	return (envp);
}

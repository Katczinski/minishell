#include "builtins.h"

char	*save_name(char *envp_str, t_info *info)
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
		print_error(strerror(errno), info, 0);
		return (0);
	}
	name = ft_memcpy(name, envp_str, i);
	return (name);
}

int	find_envp(char *name, char **envp)
{
	int	i;
	int	len;

	i = -1;
	len = ft_strlen(name);
	while (envp[++i])
	{
		if (!ft_strncmp(envp[i], name, len)
			&& value_found(envp[i]) && envp[i][len] == '=')
			break ;
		else if (!ft_strncmp(envp[i], name, len)
			&& !value_found(envp[i]) && !envp[i][len])
			break ;
	}
	if (!envp[i])
		return (0);
	return (1);
}

int	value_found(char *cmd)
{
	int	i;

	i = 0;
	while (cmd[i] && cmd[i] != '=')
		i++;
	if (cmd[i] == '=')
		return (1);
	return (0);
}

int	change_value(char *env_name, char *cmd, char **envp, t_info *info)
{
	int		i;
	char	*name;

	i = 0;
	name = 0;
	if (env_name && cmd)
	{
		while (envp[i] && ft_strncmp(envp[i], env_name, ft_strlen(env_name)))
			i++;
		if (!envp[i])
			return (print_error(strerror(errno), info, 0));
		name = save_name(envp[i], info);
		if (value_found(cmd) && !strcmp(env_name, name))
		{
			free(envp[i]);
			envp[i] = ft_strdup(cmd);
		}
		free(name);
	}
	return (0);
}

char	**create_envp(char *cmd, char **envp, t_info *info)
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
			print_error(strerror(errno), info, 0);
			return (0);
		}
		new_envp[i + 1] = 0;
		new_envp = copy_line_to_new_envp(new_envp, envp, cmd, info);
	}
	free_arr(envp);
	return (new_envp);
}

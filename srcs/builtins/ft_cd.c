#include "builtins.h"

static int	go_home(char **path, char **envp, t_info *info)
{
	char	*home_dir;
	int		i;

	i = 0;
	home_dir = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "HOME=", 5))
		{
			home_dir = envp[i];
			break ;
		}
		i++;
	}
	if (!home_dir)
		return (print_error("minishell: cd: HOME not set", info, 0));
	while (*home_dir != '=')
		home_dir++;
	home_dir++;
	*path = home_dir;
	return (0);
}

int	save_oldpwd(char ***envp, t_info *info)
{
	char	*dir_name;
	char	*line_to_save;

	dir_name = 0;
	line_to_save = 0;
	dir_name = getcwd(dir_name, BUFFER_SIZE);
	if (!dir_name)
	{
		print_error(strerror(errno), info, 0);
		return (0);
	}
	line_to_save = ft_strjoin("OLDPWD=", dir_name);
	if (!line_to_save)
	{
		print_error(strerror(errno), info, 0);
		return (0);
	}
	*envp = add_envp(line_to_save, *envp, info);
	free(dir_name);
	free(line_to_save);
	return (1);
}

int	ft_cd(t_command_list *list, char ***envp, t_info *info)
{
	int		ret;
	char	*path;

	ret = 0;
	if (list->lines < 3)
	{
		if (!save_oldpwd(envp, info))
			return (1);
		if (list->lines == 1)
		{
			ret = go_home(&path, *envp, info);
			if (ret)
				return (ret);
		}
		else
			path = list->command[1];
		if (chdir(path) == -1)
			return (print_error(strerror(errno), info, 0));
	}
	else
		return (print_error("minishell: cd: to many arguments", info, 0));
	return (ret);
}

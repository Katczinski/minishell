#include "minishell.h"

extern t_all    g_all;

char	**get_path(char **envp)
{
	char	**path;

	if (g_all.path)
		free_darr((void **)g_all.path);
	g_all.path = 0;
	while (*envp != NULL && ft_strncmp(*envp, "PATH=", 5))
		envp++;
	if (*envp == NULL)
		return (0);
	path = ft_split(*envp + 5, ':');
	return (path);
}

void	get_binary(t_command_list *cmd)
{
	char		*temp;
	int			i;
	struct stat	*stats;

	stats = (struct stat *)malloc(sizeof(struct stat));
	i = 0;
	while (g_all.path && g_all.path[i])
	{
		temp = ft_strjoin(g_all.path[i], "/");
		if (!temp)
			exit(1);
		g_all.binary = ft_strjoin(temp, cmd->command[0]);
		free(temp);
		if (!stat(g_all.binary, stats))
			break ;
		free(g_all.binary);
		g_all.binary = 0;
		i++;
	}
	if (!g_all.binary)
		if (!stat(cmd->command[0], stats) && !(S_ISDIR(stats->st_mode))
			&& ft_strchr(cmd->command[0], '/'))
			g_all.binary = ft_strdup(cmd->command[0]);
	free(stats);
}


char	*get_shlvl(char *envp)
{
	int		shlvl;
	int		i;
	char	*str;
	char	*save;
	char	*lvl;
	
	i = 0;
	str = malloc(sizeof(char) * 7);
	while ('0' > *envp || *envp > '9')
	{
		str[i] = *envp;
		envp++;
		i++;
	}
	str[i] = '\0';
	shlvl = ft_atoi(envp) + 1;
	save = str;
	lvl = ft_itoa(shlvl);
	str = ft_strjoin(str, lvl);
	free(lvl);
	free(save);
	return (str);
	
}

char	**save_envp(char **envp)
{
	int		i;
	char	**new_envp;
	int		shlvl;

	shlvl = 1;
	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "SHLVL=", 6))
			shlvl--;
		i++;
	}
	new_envp = malloc(sizeof(char *) * (i + shlvl + 1));
	new_envp[i + shlvl] = 0;
	i = -1;
	while (envp[++i])
	{
		if (!ft_strncmp(envp[i], "SHLVL=", 6))
			new_envp[i] = get_shlvl(envp[i]);
		else
			new_envp[i] = ft_strdup(envp[i]);
	}
	if (shlvl == 1)
		new_envp[i] = get_shlvl("SHLVL=0");

//
	// new_envp = malloc(sizeof(char *) * 5);
	// new_envp[0] = strdup("a=0");
	// new_envp[1] = strdup("aaa=1");
	// new_envp[2] = strdup("abc=2");
	// new_envp[3] = strdup("abcd=3");
	// new_envp[4] = 0;
	// while (envp[i])
	// 	i++;

//
	return (new_envp);
}
#include "builtins.h"

int	find_value(char *envp)
{
	int	i;

	i = 0;
	while (envp[i] && envp[i] != '=')
		i++;
	if (!envp[i])
		return (1);
	return (0);
}

int	ft_env(char **envp)
{
	int	i;

	i = -1;
	if (envp)
	{
		while (envp[++i])
		{
			if (!find_value(envp[i]))
				printf("%s\n", envp[i]);
		}
	}
	return (0);
}
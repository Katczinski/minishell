#include "builtins.h"

int	ft_env(char **envp)
{
	int	i;

	i = -1;
	if (envp)
	{
		while (envp[++i])
			printf("%s\n", envp[i]);
	}
	return (0);
}
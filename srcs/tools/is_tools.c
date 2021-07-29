#include "minishell.h"

int	is_redir(int type)
{
	if (type == RED_IN || type == DRED_IN
		|| type == RED_OUT || type == DRED_OUT)
		return (1);
	return (0);
}

int	is_all_whitespaces(char *str)
{
	int	i;

	i = 0;
	while (str && str[i] != '\0')
	{
		if ((str[i] < 9 || str[i] > 14) && str[i] != ' ')
			return (0);
		i++;
	}
	return (1);
}

int	is_builtin(int type)
{
	if (type == FT_ECHO || type == FT_PWD || type == FT_CD
		|| type == FT_EXPORT || type == FT_UNSET || type == FT_ENV
		|| type == FT_EXIT)
		return (1);
	return (0);
}

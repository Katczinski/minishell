#include "libft.h"

void	ft_skip_whitespaces(int *i, char *line)
{
	// if (*i < 0 && (line[*i + 1] == '\t' || line[*i + 1] == '\n' || line[*i + 1] == '\r'
	// || line[*i + 1] == '\v' || line[*i + 1] == '\f' || line[*i + 1] == ' '))
	// 	*i = 0;
	while (line[*i] == '\t' || line[*i] == '\n' || line[*i] == '\r'
	|| line[*i] == '\v' || line[*i] == '\f' || line[*i] == ' ')
		(*i)++;
	// if (line[*i - 1] && (line[*i - 1] == '\t' || line[*i - 1] == '\n' || line[*i - 1] == '\r'
	// || line[*i - 1] == '\v' || line[*i - 1] == '\f' || line[*i - 1] == ' '))
	// 	(*i)--;
}
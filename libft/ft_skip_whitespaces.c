#include "libft.h"

void	ft_skip_whitespaces(int *i, char *line)
{
	while (line[*i] == '\t' || line[*i] == '\n' || line[*i] == '\r'
	|| line[*i] == '\v' || line[*i] == '\f' || line[*i] == ' ')
		(*i)++;
}
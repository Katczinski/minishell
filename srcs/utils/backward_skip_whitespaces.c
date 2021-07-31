#include "parser.h"

int	skip_whitespaces(int i, char *line)
{
	while (line[i] && (line [i] == '>' || line [i] == '<'))
		i--;
	while (line[i] && (line[i] == '\t' || line[i] == '\n'
			|| line[i] == '\r' || line[i] == '\v'
			|| line[i] == '\f' || line[i] == ' '))
		i--;
	if (line[i] || !line[i] || line[i] == '|'
		|| line [i] == '>' || line [i] == '<')
		return (1);
	return (0);
}

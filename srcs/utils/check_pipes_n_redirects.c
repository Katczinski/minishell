#include "parser.h"

int	skip_whitespaces(int i, char *line)
{
	while (line[i] && (line [i] == '>' || line [i] == '<'))
	 	i--;
	while (line[i] && (line[i] == '\t' || line[i] == '\n' || line[i] == '\r'
	|| line[i] == '\v' || line[i] == '\f' || line[i] == ' '))
		i--;
	if (line[i] || !line[i] || line[i] == '|' || line [i] == '>' || line [i] == '<')
		return (1);
	return (0);
}

// int check_pipes_n_redirects(char *line)
// {
// 	int i;
// 	int red;
// 	int pipe;
// 	int cmd;

// 	i = -1;
// 	red = 0;
// 	pipe = 0;
// 	cmd = 0;
// 	while (line[++i] && line[i] != '\'' && line[i] != '\"')
// 	{
// 		if (line[i] && line[i] != '\t' && line[i] != '\n' && line[i] != '\r'
// 			&& line[i] != '\v' && line[i] != '\f' && line[i] != ' ' && line[i] != '|' && line[i] != '<' && line[i] != '>')
// 		{
// 			red = 0;
// 			pipe = 0;
// 			cmd = 1;
// 		}
// 		if (line[i] == '|' && cmd)
// 		{
// 			pipe++;
// 			cmd = 0;
// 		}
// 		else if (line[i] == '|' && !cmd)
// 			return (1);
// 		if (line[i] == '<' || line[i] == '>')
// 			red++;
// 	}
// 	if (!line[i] && (red || pipe))
// 		return (1);
// 	return (0);
// }

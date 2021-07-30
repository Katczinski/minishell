#include "parser.h"

static int	space_pipe_red_check(char c)
{
	if (c != '\t' && c != '\n' && c != '\r'
		&& c != '\v' && c != '\f' && c != ' '
		&& c != '|' && c != '<' && c != '>')
		return (1);
	return (0);
}

static int	redirects_check(char *line, int i)
{
	int	in;
	int	out;

	out = 0;
	in = 0;
	while (line[i] && out < 3 && in < 3)
	{
		if (line[i] && space_pipe_red_check(line[i]) && out)
			out = 0;
		else if (line[i] && space_pipe_red_check(line[i]) && in)
			in = 0;
		if (line[i] == '\'' || line[i] == '\"' || line[i] == '|')
			break ;
		if (line[i] == '>')
			out++;
		else if (line[i] == '<')
			in++;
		if (out && in)
			return (1);
		i++;
	}
	if (out || in)
		return (1);
	return (0);
}

static int	pipes_check(char *line, int i, t_info *info)
{
	int	pipe;

	pipe = 0;
	info->elements++;
	while (line[i] && pipe < 2)
	{
		if (line[i] && line[i] != '\t' && line[i] != '\n'
			&& line[i] != '\r' && line[i] != '\v' && line[i] != '\f'
			&& line[i] != ' ' && line[i] != '|' && pipe)
			pipe = 0;
		if (line[i] == '\'' || line[i] == '\"'
			|| line[i] == '<' || line[i] == '>')
			break ;
		if (line[i] == '|')
			pipe++;
		i++;
	}
	if (pipe)
		return (1);
	return (0);
}

static int	start(char *line, t_info *info)
{
	int	i;

	i = 0;
	info->elements++;
	ft_skip_whitespaces(&i, line);
	if (i >= 0 && line[i] == '|')
		info->redirects++;
	return (i);
}

int	line_check(char *line, t_info *info)
{
	int	i;

	i = start(line, info);
	while (line[i] && !info->redirects)
	{
		if (line[i] == '\'' && !info->quotes && !info->dquotes)
			info->quotes = 1;
		else if (line[i] == '\'' && info->quotes && !info->dquotes)
			info->quotes = 0;
		else if (line[i] == '\"' && !info->dquotes && !info->quotes)
			info->dquotes = 1;
		else if (line[i] == '\"' && info->dquotes && !info->quotes)
			info->dquotes = 0;
		else if (line[i] == '|' && !info->quotes && !info->dquotes)
			info->redirects = pipes_check(line, i, info);
		else if ((line[i] == '>' || line[i] == '<')
			&& !info->quotes && !info->dquotes)
			info->redirects = redirects_check(line, i);
		i++;
	}
	if ((!line[i] && (info->quotes || info->dquotes)) || info->redirects)
		return (print_error("Wrong syntax", info, 1));
	return (0);
}

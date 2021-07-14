#include "parser.h"

static char	*ft_strcpy(char *dst, const char *src)
{
	size_t j;

	if (!dst || !src)
		return (0);
	j = 0;
	while (src[j])
	{
		dst[j] = src[j];
		j++;
	}
	dst[j] = '\0';
	return (dst);
}

static int	next_line_exists(char **next_line, char **line)
{
	char	*line_break;

	if ((line_break = ft_strchr(*next_line, '\n')))
	{
		*line_break = '\0';
		*line = ft_strdup(*next_line);
		if (*(line_break + 1))
			ft_strcpy(*next_line, ++line_break);
		else
		{
			free(*next_line);
			*next_line = NULL;
			return (0);
		}
		return (1);
	}
	else
	{
		*line = ft_strdup(*next_line);
		free(*next_line);
		*next_line = NULL;
		return (1);
	}
}

static int	next_line_checker(char **next_line, char **line)
{
	if (*next_line)
	{
		if (!(next_line_exists(next_line, line)))
			return (1);
	}
	else
		*line = (char*)ft_calloc(1, sizeof(char *));
	return (0);
}

static void	next_line_finder(char *line_break, char **next_line)
{
	*line_break = '\0';
	if (*(line_break + 1))
		*next_line = ft_strdup(++line_break);
	else
	{
		free(*next_line);
		*next_line = NULL;
	}
}

int			get_next_line(int fd, char **line)
{
	char		buf[BUFFER_SIZE + 1];
	int			byte_reader;
	char		*line_break;
	static char	*next_line;
	char		*tmp;

	if (fd < 0 || !line || BUFFER_SIZE < 1 || read(fd, 0, 0) < 0)
		return (-1);
	line_break = NULL;
	byte_reader = 1;
	if ((next_line_checker(&next_line, line)))
		return (1);
	while (!next_line && !line_break && byte_reader)
	{
		if ((byte_reader = read(fd, buf, BUFFER_SIZE)) < 0)
			return (-1);
		buf[byte_reader] = '\0';
		if ((line_break = ft_strchr(buf, '\n')))
			next_line_finder(line_break, &next_line);
		tmp = *line;
		if (!(*line = ft_strjoin(*line, buf)))
			return (-1);
		free(tmp);
	}
	return (byte_reader || ft_strlen(next_line) ? 1 : 0);
}

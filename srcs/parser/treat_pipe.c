#include "parser.h"

static int	find_red_in(char *line)
{
	int	i;

	i = 0;
	ft_skip_whitespaces(&i, line);
	if (line[i] == '<' || line[i] == '>')
		return (1);
	return (0);
}

int	copy_prev_line(t_info *info, int *i, char *line)
{
	char	*prev_str;

	if ((info->tail && (!info->tail->type || info->tail->type == RED_IN
				|| info->tail->type == DRED_IN || info->tail->type == RED_OUT
				|| info->tail->type == DRED_OUT) && *i - 1 >= 0 && line[*i - 1])
		|| (!info->tail && *i - 1 >= 0 && line[*i - 1]))
	{
		prev_str = malloc(sizeof(char) * (*i + 1));
		if (!prev_str)
		{
			print_error(strerror(errno), info, 1);
			return (0);
		}
		prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
		if (!info->tail)
			add_element(init_element(info), info);
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(prev_str, info->tail, info);
	}
	return (1);
}

char	*treat_pipe(char *line, int *i, t_info *info)
{
	char	*output;

	if (!copy_prev_line(info, i, line))
		return (0);
	if (line[*i] == '|')
		*i = (*i) + 1;
	add_element(init_element(info), info);
	info->tail->type = PIPE;
	ft_skip_whitespaces(i, line);
	output = ft_strdup(line + *i);
	if (!find_red_in(output))
		add_element(init_element(info), info);
	*i = -1;
	free(line);
	return (output);
}

#include "parser.h"

static int	find_comand(t_command_list *list)
{
	t_command_list	*tmp;

	tmp = list;
	while (tmp && tmp->type != PIPE)
	{
		if (!tmp->type)
			return (1);
		tmp = tmp->prev;
	}
	return (0);
}

char	*save_output(char *line, int *i, t_info *info)
{
	char	*file_name;
	char	*output;

	file_name = malloc(sizeof(char) * (*i + 1));
	file_name = ft_memcpy(file_name, line, *i);
	info->tail->lines++;
	info->tail->command = add_line_to_cmd(file_name, info->tail, info);
	ft_skip_whitespaces(i, line);
	output = ft_strdup(line + *i);
	if (output[0] && output[0] != '<' && output[0] != '>' && output[0] != '|')
		add_element(init_element(info), info);
	return (output);
}

char	*add_red_in(char *line, int *i, char **envp, t_info *info)
{
	char	*output;

	*i = 0;
	while (line[(*i)] && line[*i] != ' '
		&& line[*i] != '<' && line[*i] != '>'
		&& line[*i] != '|')
	{
		if (line[*i] == '\'')
			line = treat_quote(line, i, info);
		else if (line[*i] == '\"')
			line = treat_dquote(line, i, envp, info);
		else if (line[*i] == '$' && (!info->tail
				|| (info->tail && info->tail->type != DRED_IN)))
			line = treat_env(line, i, envp, info);
		(*i)++;
	}
	output = save_output(line, i, info);
	free(line);
	return (output);
}

static int	set_red_type(char *line, int *i, t_info *info)
{
	int	type;

	type = 0;
	if (line[*i] == '>' && line[*i + 1] != '>')
		type = RED_OUT;
	else if (line[*i] == '>' && line[*i + 1] == '>')
		type = DRED_OUT;
	else if (line[*i] == '<' && line[*i + 1] != '<')
		type = RED_IN;
	else if (line[*i] == '<' && line[*i + 1] == '<')
		type = DRED_IN;
	(*i)++;
	if (line[*i] == '>' || line[*i] == '<')
		(*i)++;
	add_element(init_element(info), info);
	info->tail->type = type;
	return (type);
}

char	*treat_redirect(char *line, int *i, char **envp, t_info *info)
{
	char	*output;
	int		type;

	if (!copy_prev_line(info, i, line))
		return (0);
	type = set_red_type(line, i, info);
	ft_skip_whitespaces(i, line);
	output = ft_strdup(line + *i);
	if ((type == RED_IN || type == DRED_IN || type == RED_OUT
			|| type == DRED_OUT) && (!info->tail->prev
			|| info->tail->prev->type == PIPE
			|| info->tail->prev->type == RED_IN
			|| info->tail->prev->type == DRED_IN
			|| info->tail->prev->type == RED_OUT
			|| info->tail->prev->type == DRED_OUT) && !find_comand(info->tail))
		output = add_red_in(output, i, envp, info);
	*i = -1;
	free(line);
	return (output);
}

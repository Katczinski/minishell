#include "parser.h"

int	check_last_arg(char **output, char **envp, int *i, t_info *info)
{
	while (*output && (*output)[++(*i)])
	{
		if ((*output)[*i] == ' ' || (*output)[*i] == '\t')
			*output = treat_space(*output, i, envp, info);
		if (*i >= 0 && *output && (*output)[*i]
				&& (*output)[*i] == '|')
			*output = treat_pipe(*output, i, info);
		if (*i >= 0 && *output && (*output)[*i]
				&& ((*output)[*i] == '<' || (*output)[*i] == '>'))
			*output = treat_redirect(*output, i, envp, info);
		if (*i >= 0 && *output && (*output)[*i] && (*output)[*i] == '\'')
			*output = treat_quote(*output, i, info);
		else if (*i >= 0 && *output && (*output)[*i] && (*output)[*i] == '\"')
			*output = treat_dquote(*output, i, envp, info);
		if (*i >= 0 && *output && (*output)[*i] && (*output)[*i] == '$'
				&& (!info->tail || (info->tail && info->tail->type != DRED_IN)))
			*output = treat_env(*output, i, envp, info);
	}
	if (*output && (*output)[0])
	{
		info->tail->lines++;
		return (1);
	}
	return (0);
}

static int	save_prev_line(char *line, int *i, t_info *info)
{
	char	*prev_str;

	prev_str = 0;
	if (*i > 0 && line[(*i) - 1])
	{
		prev_str = malloc(sizeof(char) * (*i + 1));
		if (!prev_str)
		{
			print_error(strerror(errno), info, 1);
			return (0);
		}
		prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
	}
	if (!info->tail && prev_str && prev_str[0])
		add_element(init_element(info), info);
	if (prev_str && prev_str[0])
	{
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(prev_str, info->tail, info);
	}
	return (1);
}

char	*treat_space(char *line, int *i, char **envp, t_info *info)
{
	char	*output;

	if (!save_prev_line(line, i, info))
		return (0);
	ft_skip_whitespaces(i, line);
	output = 0;
	output = ft_strdup(line + *i);
	free(line);
	*i = -1;
	if (output[0] && check_last_arg(&output, envp, i, info))
	{
		info->tail->command = add_line_to_cmd(output, info->tail, info);
		*i = -1;
		return (0);
	}
	return (output);
}

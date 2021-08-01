#include "parser.h"

static t_ft_dquote	*init_dquote(t_info *info, int *i)
{
	t_ft_dquote	*dquote;

	dquote = malloc(sizeof(t_ft_dquote));
	if (!dquote)
		print_error("Malloc error\n", info, 1);
	dquote->prev_str = 0;
	dquote->curr_str = 0;
	dquote->next_str = 0;
	dquote->tmp = 0;
	dquote->frst_quote = *i;
	return (dquote);
}

static void	free_dquote(t_ft_dquote *dquote, char *line)
{
	if (dquote && dquote->prev_str)
		free(dquote->prev_str);
	if (dquote && dquote->curr_str)
		free(dquote->curr_str);
	if (dquote && dquote->next_str)
		free(dquote->next_str);
	if (dquote && dquote->tmp)
		free(dquote->tmp);
	if (line)
		free(line);
	if (dquote)
	{
		dquote->prev_str = 0;
		dquote->curr_str = 0;
		dquote->next_str = 0;
		dquote->tmp = 0;
		free(dquote);
	}
}

static char	*save_output_line(t_ft_dquote *dquote, char *line, int *i,
								t_info *info)
{
	char	*output;

	output = 0;
	dquote->tmp = ft_strjoin(dquote->prev_str, dquote->curr_str);
	dquote->next_str = ft_strdup(line + *i + 1);
	output = ft_strjoin(dquote->tmp, dquote->next_str);
	*i = (*i) - 2;
	if (dquote->curr_str && !dquote->curr_str[0] && info->tail)
	{
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(dquote->curr_str,
				info->tail, info);
		dquote->curr_str = 0;
	}
	free_dquote(dquote, line);
	return (output);
}

char	*treat_dquote(char *line, int *i, char **envp, t_info *info)
{
	char		*output;
	t_ft_dquote	*dquote;

	dquote = init_dquote(info, i);
	dquote->prev_str = malloc(sizeof(char) * (*i + 1));
	dquote->prev_str = ft_memcpy(dquote->prev_str, line, (size_t)(*i));
	if (info->tail && info->tail->type == DRED_IN)
		info->tail->quoted = 1;
	while (line[++(*i)])
	{
		if (*i >= 0 && line[*i] == '\"')
		{
			dquote->curr_str = malloc(sizeof(char)
					* ((*i) - dquote->frst_quote));
			dquote->curr_str = ft_memcpy(dquote->curr_str, line
					+ dquote->frst_quote + 1, (size_t)((*i)
						- dquote->frst_quote - 1));
			break ;
		}
		if (*i >= 0 && line[*i] == '$' && (!info->tail
				|| (info->tail && info->tail->type != DRED_IN)))
			line = treat_env(line, i, envp, info);
	}
	output = save_output_line(dquote, line, i, info);
	return (output);
}

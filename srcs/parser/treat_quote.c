#include "parser.h"

static t_ft_quote	*init_quote(t_info *info, int *i)
{
	t_ft_quote	*quote;

	quote = malloc(sizeof(t_ft_quote));
	if (!quote)
		print_error("Malloc error\n", info, 1);
	quote->prev_str = 0;
	quote->curr_str = 0;
	quote->next_str = 0;
	quote->tmp = 0;
	quote->frst_quote = *i;
	return (quote);
}

static void	free_quote(t_ft_quote *quote, char *line)
{
	if (quote && quote->prev_str)
		free(quote->prev_str);
	if (quote && quote->curr_str)
		free(quote->curr_str);
	if (quote && quote->next_str)
		free(quote->next_str);
	if (quote && quote->tmp)
		free(quote->tmp);
	if (line)
		free(line);
	if (quote)
	{
		quote->prev_str = 0;
		quote->curr_str = 0;
		quote->next_str = 0;
		quote->tmp = 0;
		free(quote);
	}
}

static char	*save_output_line(t_ft_quote *quote, char *line, int *i,
								t_info *info)
{
	char	*output;

	output = 0;
	quote->tmp = ft_strjoin(quote->prev_str, quote->curr_str);
	quote->next_str = ft_strdup(line + *i + 1);
	output = ft_strjoin(quote->tmp, quote->next_str);
	if (!quote->curr_str[0] && info->tail)
	{
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(quote->curr_str,
				info->tail, info);
		quote->curr_str = 0;
	}
	*i = (*i) - 2;
	free_quote(quote, line);
	return (output);
}

char	*treat_quote(char *line, int *i, t_info *info)
{
	char		*output;
	t_ft_quote	*quote;

	quote = init_quote(info, i);
	quote->prev_str = malloc(sizeof(char) * (*i + 1));
	quote->prev_str = ft_memcpy(quote->prev_str, line, (size_t)(*i));
	if (info->tail && info->tail->type == DRED_IN)
		info->tail->quoted = 1;
	while (line[++(*i)])
	{
		if (line[*i] == '\'' )
		{
			quote->curr_str = malloc(sizeof(char) * ((*i) - quote->frst_quote));
			quote->curr_str = ft_memcpy(quote->curr_str,
					line + quote->frst_quote + 1,
					(size_t)((*i) - quote->frst_quote - 1));
			break ;
		}
	}
	output = save_output_line(quote, line, i, info);
	return (output);
}

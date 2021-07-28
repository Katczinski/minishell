#include "parser.h"

int	redirects_check(char *line, int i)
{
	int in;
	int out;

	out = 0;
	in = 0;
	while (line[i] && out < 3 && in < 3)
	{
		if (line[i] && line[i] != '\t' && line[i] != '\n' && line[i] != '\r'
			&& line[i] != '\v' && line[i] != '\f' && line[i] != ' ' && line[i] != '|' && line[i] != '<' && line[i] != '>' && out)
			out = 0;
		else if (line[i] && line[i] != '\t' && line[i] != '\n' && line[i] != '\r'
			&& line[i] != '\v' && line[i] != '\f' && line[i] != ' ' && line[i] != '|' && line[i] != '<' && line[i] != '>' && in)
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

int	pipes_check(char *line, int i)
{
	int pipe;

	pipe = 0;
	while (line[i] && pipe < 2)
	{
		if (line[i] && line[i] != '\t' && line[i] != '\n' && line[i] != '\r'
			&& line[i] != '\v' && line[i] != '\f' && line[i] != ' ' && line[i] != '|' && line[i] != '<' && line[i] != '>' && pipe)
			pipe = 0;
		if (line[i] == '\'' || line[i] == '\"' || line[i] == '<' || line[i] == '>')
			break ;
		if (line[i] == '|')
			pipe++;
		i++;
	}
	if (pipe)
		return (1);
	return (0);
}

static int	line_check(char *line, t_info *info)
{
    int i;
    int quotes;
    int dquotes;
	int redirects;

    // if (*line == '\0')
    //     return ;
    i = 0;
    quotes = 0;
    dquotes = 0;
	redirects = 0;
    // if (line[0] && !ft_isalnum(line[0]) && line[0] != '<' && line[0] != '>'
	// && line[0] != '\"' && line[0] != '\'' && line[0] != ' '	&& line[0] != '\t' && line[0] != '.' && line[0] != '/' && line[0] != '$')
	// 	return (print_error("Wrong syntax", info, 1));
	info->elements++;
	ft_skip_whitespaces(&i, line);
	if (i >= 0 && line[i] == '|')
		redirects++;
    while (line[i] && !redirects)
    {
        if (line[i] == '\'' && !quotes && !dquotes)
			quotes = 1;
        else if (line[i] == '\'' && quotes && !dquotes)
            quotes = 0;
        else if (line[i] == '\"' && !dquotes && !quotes)
			dquotes = 1;
        else if (line[i] == '\"' && dquotes && !quotes)
            dquotes = 0;
        else if (line[i] == '|' && !quotes && !dquotes)
		{
			redirects = pipes_check(line, i);
            info->elements++;
		}
		else if ((line[i] == '>' || line[i] == '<') && !quotes && !dquotes)
			redirects = redirects_check(line, i);
		i++;
		// if (redirects)
		// 	break ;
    }
    if ((!line[i] && (quotes || dquotes)) || redirects)
		return (print_error("Wrong syntax", info, 1));
	return (0);
}

// char	**check_tabs(char **line)
// {
// 	int i;
// 	int j;

// 	i = -1;
// 	j = -1;
// 	while (line[++i])
// 	{
// 		j = -1;
// 		while (line[i][++j])
// 			if (line[i][j] == '\a')
// 				line[i][j] = ' ';
// 	}
// 	return (line);
// }

// char *backslash(char *line, int *i)
// {
//     char *prev_str;
// 	char *next_str;
// 	char *output;

//     prev_str = malloc(sizeof(char) * (*i + 1));
//     prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
// 	next_str = ft_strdup(line + *i + 1);
// 	output = ft_strjoin(prev_str, next_str);
// 	free(line);
//     // printf("out: %s\n", output);
//     return (output);
// }

void ft_free_lines(char *str1, char *str2, char *str3, char *str4)
{
	if (str1)
		free(str1);
	if (str2)
		free(str2);
	if (str3)
		free(str3);
	if (str4)
		free(str4);
}

char *treat_env(char *line, int *i, char **envp, t_info *info)
{
    int start;
    int j;
    char *prev_str;
	char *curr_str;
	char *key;
	char *next_str;
	char *output;
	char *tmp;

    start = *i;
    if (line[start + 1] != '_' && line[start + 1] != '?' && !ft_isalnum(line[start + 1]))
        return (line);
    prev_str = malloc(sizeof(char) * (*i + 1));
    prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
    while (line[++(*i)])
    {
        if ((line[*i + 1] != '_' && !ft_isalnum(line[*i + 1])) || (line[*i] != '_' && !ft_isalnum(line[*i])))
        {
            key = malloc(sizeof(char) * ((*i) - start + 1));
    		key = ft_memcpy(key, line + start + 1, (size_t)((*i) - start));
			break ;
        }
    }
	next_str = strdup(line + *i + 1);
	// printf("next str is: %s\n", next_str);
    j = -1;
	curr_str = 0;
    while (envp[++j])
    {
		// printf("%s\n", envp[j]);
        if (!ft_strncmp(key, envp[j], ft_strlen(key)) && envp[j][ft_strlen(key)] == '=')
        {
            curr_str = malloc(sizeof(char) * (ft_strlen(envp[j]) - ft_strlen(key)) + 1);
			curr_str = ft_memcpy(curr_str, envp[j] + ft_strlen(key) + 1, ft_strlen(envp[j]) - ft_strlen(key));
            break ;
        }
    }
	if (key[0] == '?' && !key[1] && !curr_str)
		curr_str = ft_itoa(info->status);
	if (!curr_str && !envp[j])
	{
		*i = start - 1;
		output = ft_strjoin(prev_str, next_str);
		ft_free_lines(prev_str, curr_str, next_str, key);
		free(line);
		return (output);
	}
	tmp = ft_strjoin(prev_str, curr_str);
	output = ft_strjoin(tmp, next_str);
	free(tmp);
	if (curr_str)
		*i = ft_strlen(curr_str) - 1;
	ft_free_lines(prev_str, curr_str, next_str, key);
		free(line);
    return (output);
}

char *treat_quote(char *line, int *i, t_info *info)
{
	int		frst_quote;
    char	*prev_str;
	char	*curr_str = NULL;
	char	*next_str;
	char	*output;
	char	*tmp;

	frst_quote = *i;
    prev_str = malloc(sizeof(char) * (*i + 1));
    prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
	if (info->tail && info->tail->type == DRED_IN)
		info->tail->quoted = 1;
    while (line[++(*i)])
    {
        if (line[*i] == '\'' )
		{
			curr_str = malloc(sizeof(char) * ((*i) - frst_quote));
    		curr_str = ft_memcpy(curr_str, line + frst_quote + 1, (size_t)((*i) - frst_quote - 1));
			break;
        // if (line[*i] == '\\' && (line[*i + 1] == '\\' || line[*i + 1] == '\'' || line[*i + 1] == '$'))
        //     line = backslash(line, i);
		}
    }
	tmp = ft_strjoin(prev_str, curr_str);
	next_str = ft_strdup(line + *i + 1);
	output = ft_strjoin(tmp, next_str);
	free(tmp);
	*i = (*i) - 2;
	ft_free_lines(prev_str, curr_str, next_str, line);
    return (output);
}

char *treat_dquote(char *line, int *i, char **envp, t_info *info)
{
	int frst_quote;
    char *prev_str;
	char *curr_str = NULL;
	char *next_str;
	char *output;
	char *tmp;

	frst_quote = *i;
    prev_str = malloc(sizeof(char) * (*i + 1));
    prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
	if (info->tail && info->tail->type == DRED_IN)
		info->tail->quoted = 1;
    while (line[++(*i)])
    {
		if (line[*i] == '$' && (!info->tail || (info->tail && info->tail->type != DRED_IN)))
			line = treat_env(line, i, envp, info);
		// if (line[*i] == ' ')
		// 	line[*i] = '\a';
        if (line[*i] == '\"')
		{
			curr_str = malloc(sizeof(char) * ((*i) - frst_quote));
    		curr_str = ft_memcpy(curr_str, line + frst_quote + 1, (size_t)((*i) - frst_quote - 1));
			break;
		}
        // if (line[*i] == '\\' && (line[*i + 1] == '\"' || line[*i + 1] == '\\' || line[*i + 1] == '$'))
        //     line = backslash(line, i);
            
    }
	tmp = ft_strjoin(prev_str, curr_str);
	if (!tmp)
		print_error(strerror(errno), info, 1);
	next_str = ft_strdup(line + *i + 1);
	if (!next_str)
		print_error(strerror(errno), info, 1);
	output = ft_strjoin(tmp, next_str);
	if (!output)
		print_error(strerror(errno), info, 1);
	free(tmp);
	// printf("char is %c\n", output[5]);
	*i = (*i) - 2;
	ft_free_lines(prev_str, curr_str, next_str, line);
    return (output);
}

int find_red_in(char *line)
{
	int i;

	i = 0;
	ft_skip_whitespaces(&i, line);
	if (line[i] == '<')
		return (1);
	return (0);
}

char	*treat_pipe(char *line, int *i, t_info *info)
{
	char	*output;
	char	*prev_str;

	if ((info->tail && (!info->tail->type || info->tail->type == RED_IN || info->tail->type == DRED_IN
		|| info->tail->type == RED_OUT || info->tail->type == DRED_OUT) && *i - 1 >= 0 && line[*i - 1]) || (!info->tail && *i - 1 >= 0 && line[*i - 1]))
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
	if (line[*i] == '|')
		*i = (*i) + 1;
	add_element(init_element(info), info);
	info->tail->type = PIPE;
	ft_skip_whitespaces(i, line);
	output = ft_strdup(line + *i);
	if (!find_red_in(output))
		add_element(init_element(info), info);
	*i = -1;
	ft_free_lines(line, 0, 0, 0);
	return (output);
}

char	*add_red_in(char *line, int *i, char **envp, t_info *info)
{
	// int start;
	char *file_name;
	char *output;

	// while (line[*i] == '<')
	// 	(*i)++;
	// ft_skip_whitespaces(i, line);
	// start = *i;
	*i = 0;
	while (line[(*i)] && line[*i] != ' ' && line[*i] != '<' && line[*i] != '>' && line[*i] != '|')
	{
		if (line[*i] == '\'')
			line = treat_quote(line, i, info);
		else if (line[*i] == '\"')
			line = treat_dquote(line, i, envp, info);
		else if (line[*i] == '$' && (!info->tail || (info->tail && info->tail->type != DRED_IN)))
			line = treat_env(line, i, envp, info);
		(*i)++;
	}
	file_name = malloc(sizeof(char) * (*i + 1));
	file_name = ft_memcpy(file_name, line, *i);
	info->tail->lines++;
	info->tail->command = add_line_to_cmd(file_name, info->tail, info);
	ft_skip_whitespaces(i, line);
	output = ft_strdup(line + *i);
	if (output[0] && output[0] != '<' && output[0] != '>' && output[0] != '|')
		add_element(init_element(info), info);
	ft_free_lines(line, 0, 0, 0);
	return (output);
}

int	find_comand(t_command_list *list)
{
	t_command_list *tmp;

	tmp = list;
	while (tmp && tmp->type != PIPE)
	{
		if (!tmp->type)
			return (1);
		tmp = tmp->prev;
	}
	return (0);
}

char *treat_redirect(char *line, int *i, char **envp, t_info *info)
{
	char	*output;
	char	*prev_str;
	int		type;

	if ((info->tail && (!info->tail->type || info->tail->type == RED_IN || info->tail->type == DRED_IN
		|| info->tail->type == RED_OUT || info->tail->type == DRED_OUT) && *i - 1 >= 0 && line[*i - 1]) || (!info->tail && *i - 1 >= 0 && line[*i - 1]))
	// if ((info->tail && !info->tail->type && line[*i - 1]) || (!info->tail && line[*i - 1]))
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
	ft_skip_whitespaces(i, line);
	output = ft_strdup(line + *i);
	if ((type == RED_IN || type == DRED_IN || type == RED_OUT || type == DRED_OUT) && (!info->tail->prev
	|| info->tail->prev->type == PIPE || info->tail->prev->type == RED_IN || info->tail->prev->type == DRED_IN
	|| info->tail->prev->type == RED_OUT || info->tail->prev->type == DRED_OUT) && !find_comand(info->tail))
		output = add_red_in(output, i, envp, info);
	*i = -1;
	ft_free_lines(line, 0, 0, 0);
	return (output);
}

int	check_last_arg(char **output, char **envp, int *i, t_info *info)
{
	if (!(*output))
		return (0);
	while (*output && (*output)[++(*i)])
	{
		if ((*output)[*i] == ' ' || (*output)[*i] == '\t')
		{
			// (*i) = (*i) - 1;
			*output = treat_space(*output, i, envp, info);
			// return (0);
		}	
		if (*i >= 0 && *output && (*output)[*i] && (*output)[*i] == '|')
		{
			*output = treat_pipe(*output, i, info);
			// break ;
		}
		if (*i >= 0 && *output && (*output)[*i] && ((*output)[*i] == '<' || (*output)[*i] == '>'))
			*output = treat_redirect(*output, i, envp, info);
		if (*i >= 0 && *output && (*output)[*i] && (*output)[*i] == '\'')
			// break ;
			*output = treat_quote(*output, i, info);
		else if (*i >= 0 && *output && (*output)[*i] && (*output)[*i] == '\"')
			*output = treat_dquote(*output, i, envp, info);
			// break ;
		if (*i >= 0 && *output && (*output)[*i] && (*output)[*i] == '$' && (!info->tail || (info->tail && info->tail->type != DRED_IN)))
			*output = treat_env(*output, i, envp, info);
	}
	if (*output && (*output)[0])
	{
		info->tail->lines++;
		return (1);
	}
	return (0);
}

char *treat_space(char *line, int *i, char **envp, t_info *info)
{
	char *output;
	char *prev_str;

	prev_str = 0;
	// printf("before removing space: %s\n", line);
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
	// printf("to add: %s\n", prev_str);
	if (!info->tail && prev_str && prev_str[0])
		add_element(init_element(info), info);
	if (prev_str && prev_str[0])
	{
		info->tail->lines++;
	// printf("afte init: hello\n");
		info->tail->command = add_line_to_cmd(prev_str, info->tail, info);
	}
	ft_skip_whitespaces(i, line);
	// printf("after skipping space: %s\n", line + (*i));
	output = ft_strdup(line + *i);
	free(line);
	*i = -1;
	// if (info->tail && info->tail->prev && (info->tail->type == RED_IN || info->tail->type == DRED_IN || info->tail->type == RED_OUT || info->tail->type == DRED_OUT))
	// {
	// 	info->tail->prev->lines++;
	// 	info->tail->prev->command = add_line_to_cmd(output, info->tail->prev, info);
	// 	return (output);
	// }
    // printf("out: %s\n", output);
	if (output[0] && check_last_arg(&output, envp, i, info))
	{
		info->tail->command = add_line_to_cmd(output, info->tail, info);
		*i = -1;
		return (0);
	}
	
	// ft_skip_whitespaces(i, line);

	//
	if (!output)
		return (0);
	return (output);
}

void	set_types(t_info *info)
{
	t_command_list *tmp;

	tmp = info->head;
	while (tmp)
	{
		if (!tmp->type && !ft_strcmp(tmp->command[0], "echo"))
			tmp->type = FT_ECHO;
		if (!tmp->type && !ft_strcmp(tmp->command[0], "cd"))
			tmp->type = FT_CD;
		if (!tmp->type && !ft_strcmp(tmp->command[0], "pwd"))
			tmp->type = FT_PWD;
		if (!tmp->type && !ft_strcmp(tmp->command[0], "export"))
			tmp->type = FT_EXPORT;
		if (!tmp->type && !ft_strcmp(tmp->command[0], "unset"))
			tmp->type = FT_UNSET;
		if (!tmp->type && !ft_strcmp(tmp->command[0], "env"))
			tmp->type = FT_ENV;
		if (!tmp->type && !ft_strcmp(tmp->command[0], "exit"))
			tmp->type = FT_EXIT;
		if (!tmp->type)
			tmp->type = COMMAND;
		tmp = tmp->next;
	}
}

void	print_list(t_info *info)
{
	int j;
	int f;

	f = 0;
	t_command_list *tmp = info->head;
	while (tmp)
	{
		printf("node: %d\n", ++f);
	  	printf("type %d\n", tmp->type);
		j = 0;
		if (tmp->command)
		{
			while (tmp->command[j])
			{
				printf("%s\n", tmp->command[j]);
				j++;
			}
		}
		printf("------\n");
		tmp = tmp->next;
	}
}

// int		need_pre_treat(char *line, int i)
// {
// 	// int	spaces;
// 	int	args;

// 	// spaces = 0;
// 	if (skip_whitespaces(i, line))
// 		return (0);
// 	args = 1;
// 	while (line[i] && (line[i] == '>' || line[i] == '<'))
// 		i++;
// 	ft_skip_whitespaces(&i, line);
// 	while (line[i] && line[i] != '|' && line[i] != '<' && line[i] != '>')
// 	{
// 		if ((line[i] == '\t' || line[i] == '\n' || line[i] == '\r'
// 		|| line[i] == '\v' || line[i] == '\f' || line[i] == ' ') 
// 		&& (line[i + 1] != '\t' && line[i + 1] != '\n' && line[i + 1] != '\r'
// 		&& line[i + 1] != '\v' && line[i + 1] != '\f' && line[i + 1] != ' ')
// 		&& (line[i + 1] != '|' && line[i + 1] != '<' && line[i + 1] != '>'))
// 		{
// 			args++;
// 			ft_skip_whitespaces(&i, line);
// 		}
// 		if (line[i] && line[i] != '|' && line[i] != '<' && line[i] != '>')
// 			i++;
// 	}
// 	printf("args: %d\n", args);
// 	if (args > 1)
// 		return (1);
// 	return (0);
// }

// char *swap_parts(char *line, int *i)
// {
// 	int		start;
// 	char	*prev_str;
// 	char	*curr_str;
// 	char	*next_str;
// 	char	*output;

// 	start = *i;
//     prev_str = malloc(sizeof(char) * (*i + 1));
//     prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
//     while (line[++(*i)])
//     {
//         if ((line[*i] == '\t' || line[*i] == '\n' || line[*i] == '\r'
// 		|| line[*i] == '\v' || line[*i] == '\f' || line[*i] == ' '))
// 		{
// 			curr_str = malloc(sizeof(char) * ((*i) - start));
//     		curr_str = ft_memcpy(curr_str, line + start + 1, (size_t)((*i) - start - 1));
// 			break ;
// 		}
//     }
// 	output = ft_strjoin(prev_str, curr_str);
// 	next_str = ft_strdup(line + *i + 1);
// 	output = ft_strjoin(output, next_str);
// 	*i = (*i) - 2;
// 	free(line);
//     return (output);
// }

// char	*pre_treat(char *line)
// {
// 	int i;

// 	i = 0;
// 	while (line[i])
// 	{
// 		if ((line[i] == '>' || line[i] == '<') && need_pre_treat(line, i))
// 			printf("hello\n");
// 			// line = swap_parts(line, &i);
// 		// else
// 			i++;
// 	}



// 	return (line);
// }

void	throw_args_to_cmd(t_command_list *list, t_info *info)
{
	t_command_list	*tmp;
	int				i;

	tmp = list;
	while (tmp)
	{
		if (tmp->type == COMMAND || tmp->type == FT_ECHO || tmp->type == FT_CD
			|| tmp->type == FT_PWD || tmp->type == FT_EXPORT || tmp->type == FT_UNSET
			|| tmp->type == FT_EXIT || tmp->type == FT_ENV)
			break;
		tmp = tmp->prev;
	}
	i = 1;
	if (tmp && tmp->command)
	{
		while (i < list->lines)
		{
			tmp->lines++;
			tmp->command = add_line_to_cmd(list->command[i], tmp, info);
			// free(list->command[i]);
			i++;
		}
		list->lines = 1;
		list->command[1] = 0;
	}
}

void	post_treat(t_info *info)
{
	t_command_list *tmp;

	tmp = info->head;
	while (tmp)
	{
		if ((tmp->type == RED_IN || tmp->type == DRED_IN || tmp->type == RED_OUT
			|| tmp->type == DRED_OUT) && tmp->lines > 1)
			throw_args_to_cmd(tmp, info);
		tmp = tmp->next;
	}
}

// int quotes_only(char *line, t_info *info)
// {
// 	int i;
// 	t_command_list *tmp;

// 	i = 0;
// 	ft_skip_whitespaces(&i, line);
// 	if (line[i] == '\'' && line[i + 1] == '\'')
// 	{
// 		i++;
// 		ft_skip_whitespaces(&i, line);
// 	}
// 	if (!line[i])
// 	{
// 		add_element(init_element(info), info);
// 		tmp = info->head;
// 		tmp->lines++;
// 		tmp->command = add_line_to_cmd("\0", tmp, info);
// 		return (1);
// 	}
// 	return (0);
// }

t_info *parser(char *line, char **envp, int status)
{
	t_info *info;
	info = init_struct(status);
    int i;
    if (line_check(line, info))
		return (0);
	// if (quotes_only(line, info))
	// 	return (info);
	// line = pre_treat(line);
    i = -1;
    while(line && line[0] && line[++i])
    {
		// printf("char: %c\n", line[i]);
		if (line && i >= 0 && (line[i] == ' ' || line[i] == '\t'))
			line = treat_space(line, &i, envp, info);
        if (line && i >= 0 && line[i] == '\'')
            line = treat_quote(line, &i, info);
        else if (line && i >= 0 && line[i] == '\"')
            line = treat_dquote(line, &i, envp, info);
        // if (line[i] == '\\')
        //     line = backslash(line, &i);
        if (line && i >= 0 && line[i] == '$' && (!info->tail || (info->tail && info->tail->type != DRED_IN)))
            line = treat_env(line, &i, envp, info);
		if (line && i >= 0 && line[i] == '|')
			line = treat_pipe(line, &i, info);
		if (line && i >= 0 && (line[i] == '<' || line[i] == '>'))
			line = treat_redirect(line, &i, envp, info);
        // printf("line: %s\n", line);
    }
	//
	// int j = -1;
	// while (line[++j])
	// 	printf("%c\n", line[j]);
	//
	if (line && i > 0 && line[i - 1] && !info->head)
	{
		add_element(init_element(info), info);
		info->tail->lines++;
 		// printf("line: %s\n", line);
 		// printf("line: %d\n", i);
		info->tail->command = add_line_to_cmd(line, info->tail, info);
	}
	// else if (info->tail && info->tail->prev && line[i - 1] && !info->tail->lines && (info->tail->prev->type == RED_IN || info->tail->prev->type == DRED_IN
	// || info->tail->prev->type == RED_OUT || info->tail->prev->type == DRED_OUT || info->tail->prev->type == PIPE))
	else if (line && info->tail && !info->tail->command)
	{
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(line, info->tail, info);
	}
	// if (line && line[0])
	// 	free(line);
	set_types(info);
	post_treat(info);
	//print_list(info);	
	return (info);
}

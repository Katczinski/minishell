#include "parser.h"

int	redirects_check(char *line, int i)
{
	int in;
	int out;

	// if (line[i] == '>')
	// 	out = 1;
	// else
	// 	in = 1;
	out = 0;
	in = 0;
	while (line[i] && out < 3 && in < 3)
	{
		if (ft_isalnum(line[i]) && out)
			out = 0;
		else if (ft_isalnum(line[i]) && in)
			in = 0;
		if (line[i] == '\'' || line[i] == '\"' || line[i] == '|')
			break ;
		// if ((line[i] == '>' && out == 2) || (line[i] == '<' && in == 2) || (line[i] == '>' && in) || (line[i] == '<' && out))
		// 	return (1);
		if (line[i] == '>')
			out++;
		else if (line[i] == '<')
			in++;
		// else if (line[i] == ' ')
		// 	ft_skip_whitespaces(&i, line);
		if (out && in)
			return (1);
		// if (line[i] == '>' && (!out || (out && line[i - 1] == '>')))
		// 	out++;
		// else if (line[i] == '<' && (!in || (in && line[i - 1] == '<')))
		// 	in++;
		
		// if ((line[i] == '>' && out) || (line[i] == '<' && in))
		// 	return (1);
		// if ((line[i] == '>' && in) || (line[i] == '<' && out))
		// 	return (1);
		
		i++;
	}
	if (out > 2 || in > 2)
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
    i = -1;
    quotes = 0;
    dquotes = 0;
	redirects = 0;
    if (line[0] && !ft_isalpha(line[0]) && line[0] != '<' && line[0] != '>'
	&& line[0] != '\"' && line[0] != '\'' && line[0] != ' '	&& line[0] != '\t')
		return (print_error("Wrong syntax\n", info));
	info->elements++;
    while (line[++i])
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
            info->elements++;
		if ((line[i] == '>' || line[i] == '<') && !quotes && !dquotes)
			redirects = redirects_check(line, i);
		if (redirects)
			break ;
    }
    if ((!line[i] && (quotes || dquotes)) || skip_whitespaces(i, line) || redirects)
		return (print_error("Wrong syntax\n", info));
	return (0);
}

char	**check_tabs(char **line)
{
	int i;
	int j;

	i = -1;
	j = -1;
	while (line[++i])
	{
		j = -1;
		while (line[i][++j])
			if (line[i][j] == '\a')
				line[i][j] = ' ';
	}
	return (line);
}

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

char *treat_env(char *line, int *i, char **envp)
{
    int start;
    int j;
    char *prev_str;
	char *curr_str;
	char *key;
	char *next_str;
	char *output;

    start = *i;
    if (line[start + 1] != '_' && !ft_isalnum(line[start + 1]))
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
        if (!ft_strncmp(key, envp[j], ft_strlen(key)) && envp[j][ft_strlen(key)] == '=')
        {
            curr_str = malloc(sizeof(char) * (ft_strlen(envp[j]) - ft_strlen(key)) + 1);
			curr_str = ft_memcpy(curr_str, envp[j] + ft_strlen(key) + 1, ft_strlen(envp[j]) - ft_strlen(key) + 1);
            break ;
        }
    }
	if (!curr_str && !envp[j])
		return (ft_strjoin(prev_str, next_str));
	output = ft_strjoin(prev_str, curr_str);
	output = ft_strjoin(output, next_str);
    return (output);
}

char *treat_quote(char *line, int *i)
{
	int		frst_quote;
    char	*prev_str;
	char	*curr_str;
	char	*next_str;
	char	*output;

	frst_quote = *i;
    prev_str = malloc(sizeof(char) * (*i + 1));
    prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
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
	output = ft_strjoin(prev_str, curr_str);
	next_str = ft_strdup(line + *i + 1);
	output = ft_strjoin(output, next_str);
	*i = (*i) - 2;
	free(line);
    return (output);
}

char *treat_dquote(char *line, int *i, char **envp, t_info *info)
{
	int frst_quote;
    char *prev_str;
	char *curr_str;
	char *next_str;
	char *output;

	frst_quote = *i;
    prev_str = malloc(sizeof(char) * (*i + 1));
    prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
    while (line[++(*i)])
    {
		if (line[*i] == '$')
			line = treat_env(line, i, envp);
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
	output = ft_strjoin(prev_str, curr_str);
	if (!output)
		print_error(strerror(errno), info);
	next_str = ft_strdup(line + *i + 1);
	if (!next_str)
		print_error(strerror(errno), info);
	output = ft_strjoin(output, next_str);
	if (!output)
		print_error(strerror(errno), info);
	// printf("char is %c\n", output[5]);
	*i = (*i) - 2;
	free(line);
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
			line = treat_quote(line, i);
		else if (line[*i] == '\"')
			line = treat_dquote(line, i, envp, info);
		else if (line[*i] == '$')
			line = treat_env(line, i, envp);
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
	free(file_name);
	return (output);
}

char *treat_redirect(char *line, int *i, char **envp, t_info *info)
{
	char	*output;
	int		type;

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
	|| info->tail->prev->type == RED_OUT || info->tail->prev->type == DRED_OUT))
		output = add_red_in(output, i, envp, info);
	*i = -1;
	free(line);
	return (output);
}

int	check_last_arg(char **output, char **envp, int *i, t_info *info)
{
	if (!(*output))
		return (0);
	while ((*output)[++(*i)])
	{
		if ((*output)[*i] == '|')
		{
			*output = treat_pipe(*output, i, info);
			// break ;
		}
		if ((*output)[*i] == '<' || (*output)[*i] == '>')
			*output = treat_redirect(*output, i, envp, info);
		if ((*output)[*i] == '\'')
			*output = treat_quote(*output, i);
		if ((*output)[*i] == '\"')
			*output = treat_dquote(*output, i, envp, info);
		if ((*output)[*i] == '$')
			*output = treat_env(*output, i, envp);
		if ((*output)[*i] == ' ' || (*output)[*i] == '\t')
		{
			(*i) = (*i) - 1;
			return (0);
		}	
	}
	if ((*output)[0])
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
	if (line[(*i) - 1])
		prev_str = malloc(sizeof(char) * (*i + 1));
	if (!prev_str)
	{
		print_error(strerror(errno), info);
		return (0);
	}
	if (line[(*i) - 1])
    	prev_str = ft_memcpy(prev_str, line, (size_t)(*i));
	// printf("to add: %s\n", prev_str);
	if (!info->tail && prev_str[0])
		add_element(init_element(info), info);
	if (prev_str[0])
	{
		info->tail->lines++;
	// printf("afte init: hello\n");
		info->tail->command = add_line_to_cmd(prev_str, info->tail, info);
	}
	ft_skip_whitespaces(i, line);
	// printf("after skipping space: %s\n", line + (*i));
	output = ft_strdup(line + *i);
	*i = -1;
    // printf("out: %s\n", output);
	if (output[0] && check_last_arg(&output, envp, i, info))
	{
		info->tail->command = add_line_to_cmd(output, info->tail, info);
	}
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
		if (!tmp->type && !ft_strncmp(tmp->command[0], "echo", ft_strlen(tmp->command[0])))
			tmp->type = FT_ECHO;
		if (!tmp->type && !ft_strncmp(tmp->command[0], "cd", ft_strlen(tmp->command[0])))
			tmp->type = FT_CD;
		if (!tmp->type && !ft_strncmp(tmp->command[0], "pwd", ft_strlen(tmp->command[0])))
			tmp->type = FT_PWD;
		if (!tmp->type && !ft_strncmp(tmp->command[0], "export", ft_strlen(tmp->command[0])))
			tmp->type = FT_EXPORT;
		if (!tmp->type && !ft_strncmp(tmp->command[0], "unset", ft_strlen(tmp->command[0])))
			tmp->type = FT_UNSET;
		if (!tmp->type && !ft_strncmp(tmp->command[0], "env", ft_strlen(tmp->command[0])))
			tmp->type = FT_ENV;
		if (!tmp->type && !ft_strncmp(tmp->command[0], "exit", ft_strlen(tmp->command[0])))
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

t_info *parser(char *line, char **envp)
{
	t_info *info;
	info = init_struct();
    int i;
    if (line_check(line, info))
		return (0);
    i = -1;
    while(line[++i])
    {
        if (line[i] == '\'')
            line = treat_quote(line, &i);
        if (line[i] == '\"')
            line = treat_dquote(line, &i, envp, info);
        // if (line[i] == '\\')
        //     line = backslash(line, &i);
        if (line[i] == '$')
            line = treat_env(line, &i, envp);
		if (line[i] == ' ' || line[i] == '\t')
			line = treat_space(line, &i, envp, info);
		if (line[i] == '|')
			line = treat_pipe(line, &i, info);
		if (line[i] == '<' || line[i] == '>')
			line = treat_redirect(line, &i, envp, info);
        // printf("line: %s\n", line);
    }
	if (line[i - 1] && !info->head)
	{
		add_element(init_element(info), info);
		info->tail->lines++;
 		// printf("line: %s\n", line);
 		// printf("line: %d\n", i);
		info->tail->command = add_line_to_cmd(line, info->tail, info);
	}
	else if (info->tail->prev && line[i - 1] && (info->tail->prev->type == RED_IN || info->tail->prev->type == DRED_IN
	|| info->tail->prev->type == RED_OUT || info->tail->prev->type == DRED_OUT || info->tail->prev->type == PIPE))
	{
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(line, info->tail, info);
	}
	set_types(info);
	//print_list(info);	
		
	return (info);
}

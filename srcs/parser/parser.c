#include "parser.h"

// void	print_list(t_info *info)
// {
// 	int				j;
// 	int				f;
// 	t_command_list	*tmp;

// 	f = 0;
// 	tmp = info->head;
// 	while (tmp)
// 	{
// 		printf("node: %d\n", ++f);
// 		printf("type %d\n", tmp->type);
// 		j = 0;
// 		if (tmp->command)
// 		{
// 			while (tmp->command[j])
// 			{
// 				printf("%s\n", tmp->command[j]);
// 				j++;
// 			}
// 		}
// 		printf("------\n");
// 		tmp = tmp->next;
// 	}
// }

void	set_types(t_info *info)
{
	t_command_list	*tmp;

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

void	post_treat(t_info *info)
{
	t_command_list	*tmp;

	tmp = info->head;
	while (tmp)
	{
		if ((tmp->type == RED_IN || tmp->type == DRED_IN || tmp->type == RED_OUT
				|| tmp->type == DRED_OUT) && tmp->lines > 1)
			throw_args_to_cmd(tmp, info);
		tmp = tmp->next;
	}
}

static void	add_last_arg(char *line, int i, t_info *info)
{
	if (line && i > 0 && line[i - 1] && !info->head)
	{
		add_element(init_element(info), info);
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(line, info->tail, info);
	}
	else if (line && info->tail && !info->tail->command)
	{
		info->tail->lines++;
		info->tail->command = add_line_to_cmd(line, info->tail, info);
	}
	set_types(info);
	post_treat(info);
}

t_info	*parser(char *line, char **envp, int status)
{
	t_info	*info;
	int		i;

	info = init_struct(status);
	if (line_check(line, info))
		return (0);
	i = -1;
	while (line && line[0] && line[++i])
	{
		if (line && i >= 0 && (line[i] == ' ' || line[i] == '\t'))
			line = treat_space(line, &i, envp, info);
		if (line && i >= 0 && line[i] == '\'')
			line = treat_quote(line, &i, info);
		else if (line && i >= 0 && line[i] == '\"')
			line = treat_dquote(line, &i, envp, info);
		if (line && i >= 0 && line[i] == '$' && (!info->tail
				|| (info->tail && info->tail->type != DRED_IN)))
			line = treat_env(line, &i, envp, info);
		if (line && i >= 0 && line[i] == '|')
			line = treat_pipe(line, &i, info);
		if (line && i >= 0 && (line[i] == '<' || line[i] == '>'))
			line = treat_redirect(line, &i, envp, info);
	}
	add_last_arg(line, i, info);
	return (info);
}

#include "parser.h"

t_ft_env	*init_env(t_info *info, int *i)
{
	t_ft_env	*env;

	env = malloc(sizeof(t_ft_env));
	if (!env)
		print_error("Malloc error\n", info, 1);
	env->prev_str = 0;
	env->curr_str = 0;
	env->next_str = 0;
	env->tmp = 0;
	env->key = 0;
	env->start = *i;
	env->j = -1;
	return (env);
}

void	free_env(t_ft_env *env, char *line)
{
	if (env && env->prev_str)
		free(env->prev_str);
	if (env && env->curr_str)
		free(env->curr_str);
	if (env && env->next_str)
		free(env->next_str);
	if (env && env->tmp)
		free(env->tmp);
	if (env && env->key)
		free(env->key);
	if (line)
		free(line);
	if (env)
	{
		env->prev_str = 0;
		env->curr_str = 0;
		env->next_str = 0;
		env->tmp = 0;
		env->key = 0;
		free(env);
	}
}

int	in_quotes(char *line, int *i)
{
	int	j;
	int	dquotes;

	j = *i;
	dquotes = 0;
	while (line[--j])
		if (line[j] == '\"')
			dquotes++;
	if (dquotes % 2)
		return (1);
	return (0);
}

#include "minishell.h"

t_command_list	*find_cmd(t_command_list *cmd)
{
	t_command_list	*start;

	start = cmd;
	while (cmd && (cmd->type != COMMAND
			&& !is_builtin(cmd->type) && cmd->type != PIPE))
		cmd = cmd->next;
	if (cmd && (cmd->type == COMMAND || is_builtin(cmd->type)))
		return (cmd);
	return (start);
}

t_command_list	*get_cmd(t_command_list *cmd)
{
	while (cmd && (cmd->type != COMMAND
			&& !is_builtin(cmd->type) && cmd->type != PIPE))
		cmd = cmd->next;
	if (cmd && (cmd->type == COMMAND || is_builtin(cmd->type)))
		return (cmd);
	return (0);
}

t_command_list	*next_pipe(t_command_list *cmd)
{
	while (cmd && cmd->type != PIPE)
		cmd = cmd->next;
	if (cmd && cmd->type == PIPE)
		return (cmd->next);
	return (0);
}
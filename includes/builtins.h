#ifndef BUILTINS_H
# define BUILTINS_H
# include <dirent.h>
# include "parser.h"

int		ft_echo(t_command_list *list);
int		ft_pwd(t_info *info);
int		ft_cd(t_command_list *list, char ***envp, t_info *info);
int		ft_export(t_command_list *list, char ***envp, t_info *info);
char	**copy_envp(char **envp, t_info *info);
void	sort_arr(char **arr);
void	print_env_lines(char **arr, int i, int j);
void	free_arr(char **arr);
int		value_found(char *cmd);
char	*save_name(char *envp_str, t_info *info);
int		find_envp(char *name, char **envp);
int		change_value(char *env_name, char *cmd, char **envp, t_info *info);
char	**create_envp(char *cmd, char **envp, t_info *info);
char	**copy_line_to_new_envp(char **new_envp,
			char **envp, char *cmd, t_info *info);
int		ft_unset(t_command_list *list, char ***envp, t_info *info);
int		find_envp(char *name, char **envp);
void	free_arr(char **arr);
int		ft_env(char **envp);
void	ft_exit(t_command_list *list, int *exit_status, int *status);

#endif

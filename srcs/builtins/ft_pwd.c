#include "builtins.h"

int ft_pwd(t_info *info)
{
    char *dir_name;

    dir_name = 0;
    if (getcwd(dir_name, BUFFER_SIZE))
        printf("%s\n", dir_name);
    else
        return (print_error(strerror(errno), info));
    return (0);
}
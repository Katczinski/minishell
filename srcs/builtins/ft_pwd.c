#include "builtins.h"

int ft_pwd(t_info *info)
{
    char *dir_name;

    dir_name = 0;
    dir_name = getcwd(dir_name, BUFFER_SIZE);
    if (dir_name)
        printf("%s\n", dir_name); 
    else
        return (print_error(strerror(errno), info, 0));
    return (0);
}
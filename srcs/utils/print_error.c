#include "parser.h"

int	print_error(char *msg, t_info *info)
{
	if (msg)
		printf("%s\n", msg);
	if (info)
		info = 0;
	return (1);
}

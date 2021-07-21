#include "parser.h"

t_info	*init_struct(int status)
{
	t_info *info;

	info = malloc(sizeof(t_info));
	if (!info)
		print_error("Malloc error\n", 0);
	info->head = 0;
	info->tail = 0;
	info->elements = 0;
	info->status = status;
	return (info);
}

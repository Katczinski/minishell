#include "parser.h"

t_info	*init_struct(int status)
{
	t_info	*info;

	info = malloc(sizeof(t_info));
	if (!info)
		print_error("Malloc error\n", 0, 1);
	info->head = 0;
	info->tail = 0;
	info->status = status;
	info->elements = 0;
	info->quotes = 0;
	info->dquotes = 0;
	info->redirects = 0;
	return (info);
}

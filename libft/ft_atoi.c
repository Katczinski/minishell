#include "libft.h"

int	ft_atoi(const char *str)
{
	long int	i;
	int			sign;

	i = 0;
	sign = 1;
	while (*str && (*str == '\t' || *str == '\f' || *str == '\v'
			|| *str == '\r' || *str == '\n' || *str == ' '))
		str++;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
		str++;
	while (*str && (*str >= '0' && *str <= '9'))
	{
		i = i * 10 + (*str - 48);
		str++;
	}
	return ((int)i * sign);
}

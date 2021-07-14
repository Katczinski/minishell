/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/29 16:26:49 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/05 13:06:55 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	ll;
	char	*res;

	res = (char *)big;
	ll = ft_strlen(little);
	if (ll == 0)
		return (res);
	i = 0;
	while (i < len && *big != '\0')
	{
		if (*big == *little && i + ll <= len)
			if (!(ft_strncmp(big, little, ll)))
				return (res + i);
		big++;
		i++;
	}
	return (NULL);
}

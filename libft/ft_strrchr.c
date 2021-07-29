/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/29 15:01:10 by abirthda          #+#    #+#             */
/*   Updated: 2020/10/31 18:55:50 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*str;
	size_t	len;

	str = (char *)s;
	len = ft_strlen(str);
	if (c == '\0')
		return (str + len);
	while (len)
	{
		if (str[len - 1] == c)
			return (&str[len - 1]);
		len--;
	}
	return (NULL);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/01 16:15:56 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/01 16:32:35 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft.h"

char	*ft_strdup(const char *s)
{
	char	*res;
	size_t	len;

	len = ft_strlen(s) + 1;
	res = (char *)malloc(sizeof(char) * len);
	if (res)
	{
		while (len)
		{
			res[len - 1] = s[len - 1];
			len--;
		}
	}
	return (res);
}

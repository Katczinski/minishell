/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/02 14:01:42 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/02 14:13:06 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*result;
	size_t	lens1;
	size_t	lens2;

	if (s1 == 0 || s2 == 0)
		return (0);
	lens1 = ft_strlen(s1) + 1;
	lens2 = ft_strlen(s2) + 1;
	result = (char *)malloc(sizeof(char) * (lens1 + lens2));
	if (result)
	{
		ft_strlcpy(result, s1, lens1);
		ft_strlcat(result, s2, lens1 + lens2);
	}
	return (result);
}

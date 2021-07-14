/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/02 13:44:57 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/02 14:01:15 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*result;

	result = (char *)malloc(sizeof(char) * (len + 1));
	if (s == 0)
		return (0);
	if (result && start < ft_strlen(s))
		ft_strlcpy(result, s + start, len + 1);
	return (result);
}

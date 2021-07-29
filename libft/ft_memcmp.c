/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/01 15:54:46 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/01 16:00:26 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	char	*ptrs1;
	char	*ptrs2;

	ptrs1 = (char *)s1;
	ptrs2 = (char *)s2;
	while (n)
	{
		if (*ptrs1 != *ptrs2)
			return ((unsigned char)*ptrs1 - (unsigned char)*ptrs2);
		ptrs1++;
		ptrs2++;
		n--;
	}
	return (0);
}

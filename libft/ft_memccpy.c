/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/01 14:14:03 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/01 14:51:35 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memccpy(void *dest, const void *src, int c, size_t n)
{
	char	*ptrd;
	char	*ptrs;

	ptrd = (char *)dest;
	ptrs = (char *)src;
	while (n)
	{
		*ptrd = *ptrs;
		n--;
		if ((unsigned char)*ptrs == (unsigned char)c)
			return (++ptrd);
		ptrd++;
		ptrs++;
	}
	return (NULL);
}

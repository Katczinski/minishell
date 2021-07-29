/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/01 14:31:54 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/01 15:49:11 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*ptrd;
	unsigned char	*ptrs;

	ptrd = (unsigned char *)dest;
	ptrs = (unsigned char *)src;
	if (dest == src || n == 0)
		return (dest);
	if (ptrs < ptrd)
	{
		while (n)
		{
			ptrd[n - 1] = ptrs[n - 1];
			n--;
		}
	}
	else
		return (ft_memcpy(dest, src, n));
	return (dest);
}

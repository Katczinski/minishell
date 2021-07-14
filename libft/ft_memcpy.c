/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/30 17:09:01 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/01 15:07:26 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char	*ptrd;
	char	*ptrs;

	ptrs = (char *)src;
	ptrd = (char *)dest;
	if (ptrs == ptrd || n == 0)
		return (dest);
	while (n)
	{
		*ptrd++ = *ptrs++;
		n--;
	}
	return (dest);
}

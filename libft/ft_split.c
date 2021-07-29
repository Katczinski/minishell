/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abirthda <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/02 16:31:26 by abirthda          #+#    #+#             */
/*   Updated: 2020/11/05 14:34:44 by abirthda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_words_counter(char const *str, char sep)
{
	int		newword;
	long	counter;

	counter = 0;
	newword = 1;
	while (*str)
	{
		if (*str == sep)
		{
			if (newword == 0)
				counter++;
			newword = 1;
		}
		else
			newword = 0;
		str++;
	}
	if (newword == 0)
		counter++;
	return (counter);
}

char	**free_mem(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr[i]);
	free(arr);
	return (NULL);
}

char	**ft_fill_res(char const *s, char c, char **result, size_t words)
{
	size_t	i;
	size_t	len;
	char	*str;

	str = (char *)s;
	i = 0;
	while (i < words)
	{
		len = 0;
		while (str[len] == c && str[len] != '\0')
			str++;
		while (str[len] != c && str[len] != '\0')
			len++;
		result[i] = ft_substr(str, 0, len);
		if (!result[i])
			return (free_mem(result));
		str += len;
		i++;
	}
	result[i] = (void *)0;
	return (result);
}

char	**ft_split(char const *s, char c)
{
	size_t	words;
	char	**result;

	if (s == 0)
		return (0);
	words = ft_words_counter(s, c);
	result = (char **)malloc(sizeof(char *) * (words + 1));
	if (!result)
		return (0);
	return (ft_fill_res(s, c, result, words));
}

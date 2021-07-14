#include "parser.h"

static int	word_counter(char const *s)
{
	int		i;
	int		counter;

	i = 0;
	counter = 0;
	while (s[i])
	{
		while (s[i] && (s[i] == ' ' || s[i] == '\t'))
			i++;
		if (s[i] && (s[i] != ' ' && s[i] != '\t'))
			counter++;
		while (s[i] && (s[i] != ' ' && s[i] != '\t'))
			i++;
	}
	return (counter);
}

static int	word_len(char const *s)
{
	int		i;
	int		len;

	i = 0;
	len = 0;
	while (s[i] && (s[i] == ' ' || s[i] == '\t'))
		i++;
	while (s[i] && (s[i] != ' ' && s[i] != '\t'))
	{
		len++;
		i++;
	}
	return (len);
}

static void	*cleaner(char **arr, int words)
{
	int		i;

	i = 0;
	while (i < words)
	{
		free(arr[i]);
		i++;
	}
	free(arr);
	return (0);
}

char	**ft_split_modified(char const *s)
{
	char	**arr;
	int		i;
	int		clmn;

	if (!s)
		return (0);
	arr = (char **)malloc(sizeof(char *) * (word_counter(s) + 1));
	if (!arr)
		return (0);
	i = 0;
	clmn = 0;
	while (clmn < word_counter(s) && s[i])
	{
		while (s[i] && (s[i] == ' ' || s[i] == '\t'))
			i++;
		arr[clmn] = ft_substr(s, i, word_len(&s[i]));
		if (arr[clmn] == 0)
			return (cleaner(arr, clmn));
		while (s[i] && (s[i] != ' ' && s[i] != '\t'))
			i++;
		clmn++;
	}
	arr[clmn] = 0;
	return (arr);
}

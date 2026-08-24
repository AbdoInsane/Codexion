/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:51 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:51 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

// Duplicate a string using ft_malloc
char	*ft_strdup(char *str, t_memory **collector)
{
	char	*dup;
	int		str_len;
	int		i;

	i = 0;
	if (!str)
		return (NULL);
	str_len = strlen(str);
	dup = (char *)ft_malloc(collector, str_len + 1);
	if (!dup)
		return (NULL);
	while (i < str_len)
	{
		dup[i] = str[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

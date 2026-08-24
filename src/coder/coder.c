/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:25 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:25 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "logger/log.h"
#include "table/table.h"

int	coder_start(t_table *table)
{
	t_coder	*coder;
	int		i;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		coder = &table->coders[i];
		if (pthread_create(&coder->thread, NULL, coder_routine, coder))
			return (1);
		i++;
	}
	return (0);
}

t_coder	*coder_init(t_table *table)
{
	t_coder	*coders;
	int		size;
	int		i;

	size = table->config->number_of_coders;
	coders = ft_malloc(&table->memory, sizeof(t_coder) * size);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < table->config->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].table = table;
		coders[i].state = WAITING;
		coders[i].last_compile_time_ms = 0;
		coders[i].d_left = &table->dongles[i];
		coders[i].d_right = &table->dongles[(i + 1) % size];
		pthread_mutex_init(&coders[i].mutex, NULL);
		i++;
	}
	return (coders);
}

void	coder_destroy(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->config->number_of_coders)
		pthread_join(table->coders[i++].thread, NULL);
}

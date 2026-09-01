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
		if (table->config->scheduler == EDF)
		{
			push_order(coder, coder->d_left);
			push_order(coder, coder->d_right);
		}
		if (pthread_create(&coder->thread, NULL, coder_routine, coder))
			return (1);
		i++;
		table->status.coders_created++;
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
	while (i < size)
	{
		coders[i].id = i + 1;
		coders[i].table = table;
		coders[i].state = WAITING;
		coders[i].compile_times = 0;
		coders[i].last_compile_time_ms = 0;
		coders[i].d_left = &table->dongles[i];
		coders[i].d_right = &table->dongles[(i + 1) % size];
		pthread_mutex_init(&coders[i].mutex, NULL);
		pthread_cond_init(&coders[i].cond, NULL);
		i++;
	}
	return (coders);
}

void	coder_destroy(t_table *table)
{
	int		i;
	int		total_coders;
	int		created_coders;
	t_coder	*coders;

	i = 0;
	coders = table->coders;
	created_coders = table->status.coders_created;
	total_coders = table->config->number_of_coders;
	while (i < total_coders)
	{
		if (i < created_coders && table->status.monitor_created)
			pthread_join(coders[i].thread, NULL);
		pthread_mutex_destroy(&coders[i].mutex);
		pthread_cond_destroy(&coders[i].cond);
		i++;
	}
}

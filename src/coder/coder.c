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

int	start_odd_coders(t_table *table)
{
	t_coder	*coder;
	int		i;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		if (i % 2 == 0)
		{
			i++;
			continue ;
		}
		coder = &table->coders[i];
		push_order(coder, coder->d_left);
		push_order(coder, coder->d_right);
		if (pthread_create(&coder->thread, NULL, coder_routine, coder))
		{
			fprintf(stderr,
				RED "Threads Error: Failed to create thread #%d\n" RESET, i
				+ 1);
			return (1);
		}
		coder->in_work = true;
		i++;
	}
	return (0);
}

int	start_even_coders(t_table *table)
{
	t_coder	*coder;
	int		i;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		if (i % 2 == 1)
		{
			i++;
			continue ;
		}
		coder = &table->coders[i];
		push_order(coder, coder->d_left);
		push_order(coder, coder->d_right);
		if (pthread_create(&coder->thread, NULL, coder_routine, coder))
		{
			fprintf(stderr,
				RED "Threads Error: Failed to create thread #%d\n" RESET, i
				+ 1);
			return (1);
		}
		coder->in_work = true;
		i++;
	}
	return (0);
}

int	coder_start(t_table *table)
{
	if (start_even_coders(table))
		return (1);
	if (start_odd_coders(table))
		return (1);
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
		coders[i].in_work = false;
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
	t_coder	*coders;

	i = 0;
	coders = table->coders;
	total_coders = table->config->number_of_coders;
	while (i < total_coders)
	{
		if (coders[i].in_work)
			pthread_join(coders[i].thread, NULL);
		pthread_mutex_destroy(&coders[i].mutex);
		pthread_cond_destroy(&coders[i].cond);
		i++;
	}
}

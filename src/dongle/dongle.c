/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:30 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:30 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder/coder.h"
#include "dongle.h"

static int	setup_dongle(t_dongle *dongle, t_table *table, int i, int size)
{
	(void)size;
	dongle->id = i;
	dongle->owner = 0;
	dongle->state = FREE;
	dongle->cooldown_end_ms = 0;
	dongle->acquire_time_ms = 0;
	pthread_cond_init(&dongle->cond, NULL);
	pthread_mutex_init(&dongle->mutex, NULL);
	dongle->heap = ft_malloc(&table->memory, sizeof(t_heap));
	if (!dongle->heap)
		return (1);
	dongle->heap->size = 0;
	dongle->heap->capacity = HEAP_SIZE;
	dongle->heap->orders = ft_malloc(&table->memory, sizeof(t_order)
			* HEAP_SIZE);
	if (!dongle->heap->orders)
		return (1);
	return (0);
}

t_dongle	*init_dongles(t_table *table)
{
	t_dongle	*dongles;
	int			size;
	int			i;

	size = table->config->number_of_coders;
	dongles = ft_malloc(&table->memory, sizeof(t_dongle) * size);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < size)
	{
		if (setup_dongle(&dongles[i], table, i, size))
			return (NULL);
		i++;
	}
	return (dongles);
}

int	acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	int			n_dongles;

	n_dongles = coder->table->config->number_of_coders;
	first = (t_dongle *[2]){coder->d_left,
		coder->d_right}[coder->id == n_dongles];
	second = (t_dongle *[2]){coder->d_right,
		coder->d_left}[coder->id == n_dongles];
	if (acquire_dongle(coder, first))
		return (1);
	if (acquire_dongle(coder, second))
		return (1);
	return (0);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner = 0;
	dongle->state = COOLDOWN;
	dongle->cooldown_end_ms = coder->last_compile_time_ms
		+ coder->table->config->dongle_cooldown;
	push_order(coder, dongle);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	destroy_dongles(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		pthread_cond_destroy(&table->dongles[i].cond);
		pthread_mutex_destroy(&table->dongles[i].mutex);
		i++;
	}
}

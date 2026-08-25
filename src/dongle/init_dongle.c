/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:36 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:36 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongle.h"

static int	setup_dongle(t_dongle *dongle, t_memory **mem, int i, int size)
{
	dongle->id = i;
	dongle->owner = 0;
	dongle->state = FREE;
	dongle->cooldown_end_ms = 0;
	pthread_mutex_init(&dongle->mutex, NULL);
	pthread_cond_init(&dongle->cond, NULL);
	dongle->heap = ft_malloc(mem, sizeof(t_heap));
	if (!dongle->heap)
		return (1);
	dongle->heap->size = 0;
	dongle->heap->capacity = size;
	dongle->heap->orders = ft_malloc(mem, sizeof(t_order) * size);
	if (!dongle->heap->orders)
		return (1);
	return (0);
}

t_dongle	*dongle_init(t_table *table)
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
		if (setup_dongle(&dongles[i], &table->memory, i, size))
			return (NULL);
		i++;
	}
	return (dongles);
}

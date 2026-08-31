/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 06:12:09 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/26 06:12:09 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder/coder.h"
#include "dongle.h"

static int	cooldown_dongle(t_coder *coder, t_dongle *dongle)
{
	while (get_time_ms() < dongle->cooldown_end_ms)
	{
		pthread_mutex_unlock(&dongle->mutex);
		if (sleep_coder_ms(coder, dongle->cooldown_end_ms - (long)get_time_ms()))
			return (pthread_mutex_lock(&dongle->mutex), 1);
		pthread_mutex_lock(&dongle->mutex);
	}
	if (dongle->state == COOLDOWN)
		dongle->state = FREE;
	return (0);
}

void	push_order(t_coder *coder, t_dongle *dongle)
{
	t_order		order;
	t_table		*table;
	t_scheduler	scheduler;

	table = coder->table;
	order = (t_order){0, 0, 0, coder->id};
	scheduler = table->config->scheduler;
	pthread_mutex_lock(&coder->mutex);
	if (coder->compile_times >= table->config->number_of_compiles_required)
	{
		pthread_mutex_unlock(&coder->mutex);
		return ;
	}
	if (scheduler == FIFO)
		order.key = get_time_ms();
	else
		order = (t_order){.key = coder->last_compile_time_ms
			+ table->config->time_to_burnout,
			.n_compiles = coder->compile_times,
			.is_odd = order.is_odd = (coder->id % 2), .id = coder->id};
	pthread_mutex_unlock(&coder->mutex);
	push_heap(dongle->heap, order);
}

int	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_order	*top_order;

	pthread_mutex_lock(&dongle->mutex);
	top_order = &dongle->heap->orders[0];
	while (!is_stop(coder->table))
	{
		if (dongle->state == COOLDOWN && cooldown_dongle(coder, dongle))
			break ;
		if (top_order->id == coder->id && dongle->state == FREE)
			break ;
		if (is_stop(coder->table))
			break ;
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	if (is_stop(coder->table))
		return (pthread_mutex_unlock(&dongle->mutex), 1);
	dongle->state = ACQUIRED;
	dongle->owner = coder->id;
	pop_heap(dongle->heap);
	dongle->acquire_time_ms = get_time_ms();
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

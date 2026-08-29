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

static int	dongle_cooldown(t_coder *coder, t_dongle *dongle)
{
	while (get_time_ms() < dongle->cooldown_end_ms)
	{
		pthread_mutex_unlock(&dongle->mutex);
		if (wait_ms(coder->table, &dongle->mutex, &dongle->cond,
				dongle->cooldown_end_ms - get_time_ms()))
			return (pthread_mutex_lock(&dongle->mutex), 1);
		pthread_mutex_lock(&dongle->mutex);
	}
	if (dongle->state == COOLDOWN)
		dongle->state = FREE;
	return (0);
}

int	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_order	*top_order;

	pthread_mutex_lock(&dongle->mutex);
	top_order = &dongle->heap->orders[0];
	while (!is_stop(coder->table))
	{
		if (dongle->state == COOLDOWN && dongle_cooldown(coder, dongle))
			return (pthread_mutex_unlock(&dongle->mutex), 1);
		if (top_order->id == coder->id && dongle->state == FREE)
			break ;
		if (is_stop(coder->table))
			return (pthread_mutex_unlock(&dongle->mutex), 1);
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

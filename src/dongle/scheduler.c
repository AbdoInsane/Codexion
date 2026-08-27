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

static void	restore_dongle(t_coder *coder, t_dongle *dongle)
{
	t_dongle	*restore;

	if (coder->d_left == coder->d_right)
		return ;
	if (dongle == coder->d_left)
		restore = coder->d_right;
	else
		restore = coder->d_left;
	pthread_mutex_lock(&restore->mutex);
	if (restore->owner == coder->id)
	{
		restore->owner = 0;
		restore->state = FREE;
		pthread_cond_broadcast(&restore->cond);
	}
	pthread_mutex_unlock(&restore->mutex);
}

static int	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_order	*top_order;

	top_order = &dongle->heap->orders[0];
	while (!is_stop(coder->table))
	{
		if (dongle->state == COOLDOWN && dongle_cooldown(coder, dongle))
			return (1);
		if (top_order->id == coder->id && dongle->state == FREE)
			break ;
		if (is_stop(coder->table))
			return (1);
		pthread_mutex_unlock(&dongle->mutex);
		restore_dongle(coder, dongle);
		pthread_mutex_lock(&dongle->mutex);
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	if (is_stop(coder->table))
		return (1);
	dongle->state = ACQUIRED;
	dongle->owner = coder->id;
	return (0);
}

int	dongle_request(t_dongle *dongle, t_coder *coder, t_scheduler scheduler)
{
	long	key;
	long	burnout_ms;

	if (scheduler == EDF)
	{
		burnout_ms = coder->table->config->time_to_burnout;
		pthread_mutex_lock(&coder->mutex);
		key = coder->last_compile_time_ms + burnout_ms;
		pthread_mutex_unlock(&coder->mutex);
	}
	else
		key = get_time_ms();
	pthread_mutex_lock(&dongle->mutex);
	push_heap(dongle->heap, key, coder->compile_times, coder->id);
	pthread_mutex_unlock(&dongle->mutex);
	usleep(100);
	pthread_mutex_lock(&dongle->mutex);
	if (acquire_dongle(coder, dongle))
		return (pthread_mutex_unlock(&dongle->mutex), 1);
	pop_heap(dongle->heap);
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

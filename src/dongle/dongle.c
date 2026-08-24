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

static int	wait_for_dongl(t_coder *coder, t_dongle *dongle)
{
	t_order	*turn;

	turn = &dongle->heap->orders[0];
	while (!is_stop(coder->table))
	{
		if (dongle->state == COOLDOWN)
		{
			while (get_time_ms() < dongle->cooldown_end_ms)
			{
				pthread_mutex_unlock(&dongle->mutex);
				if (wait_ms(coder->table, &dongle->mutex, &dongle->cond,
						dongle->cooldown_end_ms - get_time_ms()))
					return (1);
				pthread_mutex_lock(&dongle->mutex);
			}
			if (dongle->state == COOLDOWN)
				dongle->state = FREE;
		}
		if (turn->id == coder->id && dongle->state == FREE)
			break ;
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	if (is_stop(coder->table))
		return (1);
	return (0);
}

static int	dongle_request(t_dongle *dongle, t_coder *coder,
		t_scheduler scheduler)
{
	long	key;
	long	burnout_ms;

	burnout_ms = coder->table->config->time_to_burnout;
	pthread_mutex_lock(&dongle->mutex);
	if (scheduler == EDF)
	{
		key = coder->last_compile_time_ms + burnout_ms;
	}
	else
		key = get_time_ms();
	// printf(YEL "%d > %d\n" RESET, coder->id, dongle->id);
	push_heap(dongle->heap, key, coder->id);
	pthread_mutex_unlock(&dongle->mutex);
	usleep(100);
	pthread_mutex_lock(&dongle->mutex);
	if (wait_for_dongl(coder, dongle))
		return (pthread_mutex_unlock(&dongle->mutex), 1);
	// printf(GRN "%d < %d\n" RESET, coder->id, dongle->id);
	pop_heap(dongle->heap);
	dongle->state = ACQUIRED;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

int	acquire_dongles(t_coder *coder, t_scheduler sched)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->d_left->id < coder->d_right->id)
	{
		first = coder->d_left;
		second = coder->d_right;
	}
	else
	{
		first = coder->d_right;
		second = coder->d_left;
	}
	if (dongle_request(first, coder, sched))
		return (1);
	if (dongle_request(second, coder, sched))
		return (1);
	return (0);
}

void	dongle_release(t_coder *coder)
{
	long	cooldown_ms;

	// broadcast coders
	cooldown_ms = coder->table->config->dongle_cooldown;
	pthread_mutex_lock(&coder->d_left->mutex);
	coder->d_left->state = COOLDOWN;
	coder->d_left->cooldown_end_ms = get_time_ms() + cooldown_ms;
	pthread_cond_broadcast(&coder->d_left->cond);
	pthread_mutex_unlock(&coder->d_left->mutex);
	pthread_mutex_lock(&coder->d_right->mutex);
	coder->d_right->state = COOLDOWN;
	coder->d_right->cooldown_end_ms = get_time_ms() + cooldown_ms;
	pthread_cond_broadcast(&coder->d_right->cond);
	pthread_mutex_unlock(&coder->d_right->mutex);
}

void	dongle_destroy(t_table *table)
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

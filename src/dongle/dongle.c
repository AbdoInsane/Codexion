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

void	restore_dongle(t_coder *coder, t_dongle *curr_dongle)
{
	t_dongle	*dongle;

	if (curr_dongle->id == coder->d_left->id)
		dongle = coder->d_right;
	else
		dongle = coder->d_left;
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner == coder->id)
	{
		// printf(RED "%d V %d\n" RESET, coder->id, dongle->id);
		dongle->owner = 0;
		dongle->state = FREE;
		pthread_cond_broadcast(&dongle->cond);
	}
	pthread_mutex_unlock(&dongle->mutex);
}

static int	wait_cooldown(t_coder *coder, t_dongle *dongle)
{
	long	remaining;

	while (get_time_ms() < dongle->cooldown_end_ms)
	{
		remaining = dongle->cooldown_end_ms - get_time_ms();
		pthread_mutex_unlock(&dongle->mutex);
		if (wait_ms(coder->table, &dongle->mutex, &dongle->cond,
				remaining))
			return (pthread_mutex_lock(&dongle->mutex), 1);
		pthread_mutex_lock(&dongle->mutex);
	}
	if (dongle->state == COOLDOWN && get_time_ms() >= dongle->cooldown_end_ms)
		dongle->state = FREE;
	return (0);
}

static int	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	t_order	*turn;

	turn = &dongle->heap->orders[0];
	while (!is_stop(coder->table))
	{
		if (dongle->state == COOLDOWN && wait_cooldown(coder, dongle))
			return (1);
		if (turn->id == coder->id && dongle->state == FREE)
			break ;
		pthread_mutex_unlock(&dongle->mutex);
		restore_dongle(coder, dongle);
		pthread_mutex_lock(&dongle->mutex);
		if (is_stop(coder->table))
			break ;
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	if (is_stop(coder->table))
		return (1);
	return (0);
}

int	dongle_request(t_dongle *dongle, t_coder *coder, t_scheduler scheduler)
{
	long	key;
	long	burnout_ms;

	burnout_ms = coder->table->config->time_to_burnout;
	if (scheduler == EDF)
	{
		pthread_mutex_lock(&coder->mutex);
		key = coder->last_compile_time_ms + burnout_ms;
		pthread_mutex_unlock(&coder->mutex);
	}
	else
		key = get_time_ms();
	pthread_mutex_lock(&dongle->mutex);
	// printf(YEL "%ld %d > %d\n" RESET, key-coder->table->monitor->time_ms, coder->id, dongle->id);
	push_heap(dongle->heap, key, coder->id);
	pthread_mutex_unlock(&dongle->mutex);
	usleep(100);
	pthread_mutex_lock(&dongle->mutex);
	if (wait_for_dongle(coder, dongle))
		return (pthread_mutex_unlock(&dongle->mutex), 1);
	// printf(GRN "%ld %d < %d\n" RESET, key-coder->table->monitor->time_ms, coder->id, dongle->id);
	pop_heap(dongle->heap);
	dongle->state = ACQUIRED;
	dongle->owner = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
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

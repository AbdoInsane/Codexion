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

int		wait_for_cooldown(t_coder *coder, t_dongle *dongle)
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

void	restore_dongle(t_coder *coder, t_dongle *dongle)
{
	t_dongle	*restore;

	if (coder->d_left == coder->d_right)
		return;
	if (dongle == coder->d_left)
		restore = coder->d_right;
	else
		restore = coder->d_left;

	pthread_mutex_lock(&restore->mutex);
	if (restore->owner == coder->id)
	{
		// printf(RED "%d V %d > %d\n" RESET, coder->id, restore->id, dongle->id);
		restore->owner = 0;
		restore->state = FREE;
		pthread_cond_broadcast(&restore->cond);
	}
	pthread_mutex_unlock(&restore->mutex);
}

bool	is_dongles_acquired(t_coder *coder)
{
	bool	left_acquired;
	bool	right_acquired;

	if (coder->d_left == coder->d_right)
		return (false);
	// check left dongle acquisition
	pthread_mutex_lock(&coder->d_left->mutex);
	left_acquired = coder->d_left->owner == coder->id;
	pthread_mutex_unlock(&coder->d_left->mutex);

	// check right dongle acquisition
	pthread_mutex_lock(&coder->d_right->mutex);
	right_acquired = coder->d_right->owner == coder->id;
	pthread_mutex_unlock(&coder->d_right->mutex);
	return (left_acquired && right_acquired);
}

static int	wait_for_dongl(t_coder *coder, t_dongle *dongle)
{
	t_order	*turn;

	turn = &dongle->heap->orders[0];
	while (!is_stop(coder->table))
	{
		if (dongle->state == COOLDOWN && wait_for_cooldown(coder, dongle))
			return (1);
		if (turn->id == coder->id && dongle->state == FREE)
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
	return (0);
}

static int	dongle_request(t_dongle *dongle, t_coder *coder,
		t_scheduler scheduler)
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
	// printf(YEL "%d > %d : %ld\n" RESET, coder->id, dongle->id, key-coder->table->monitor->time_ms);
	fflush(stdout);
	pthread_mutex_lock(&dongle->mutex);
	push_heap(dongle->heap, key, coder->id);
	pthread_mutex_unlock(&dongle->mutex);
	usleep(100);
	pthread_mutex_lock(&dongle->mutex);
	if (wait_for_dongl(coder, dongle))
		return (pthread_mutex_unlock(&dongle->mutex), 1);
	// printf(GRN "%d < %d: %ld\n" RESET, coder->id, dongle->id, key-coder->table->monitor->time_ms);
	fflush(stdout);
	pop_heap(dongle->heap);
	dongle->state = ACQUIRED;
	dongle->owner = coder->id;
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
	while (!is_dongles_acquired(coder) && !is_stop(coder->table))
	{
		if (dongle_request(first, coder, sched))
			return (1);
		if (is_dongles_acquired(coder))
			break;
		if (dongle_request(second, coder, sched))
			return (1);
	}
	return (0);
}

void	dongle_release(t_coder *coder)
{
	long	cooldown_ms;

	// broadcast coders
	cooldown_ms = coder->table->config->dongle_cooldown;
	pthread_mutex_lock(&coder->d_left->mutex);
	coder->d_left->owner = 0;
	coder->d_left->state = COOLDOWN;
	coder->d_left->cooldown_end_ms = get_time_ms() + cooldown_ms;
	pthread_cond_broadcast(&coder->d_left->cond);
	pthread_mutex_unlock(&coder->d_left->mutex);
	pthread_mutex_lock(&coder->d_right->mutex);
	coder->d_right->owner = 0;
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

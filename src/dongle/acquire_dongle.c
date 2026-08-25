/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_dongle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 04:30:00 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/25 04:30:00 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder/coder.h"
#include "dongle.h"

bool	is_dongles_acquired(t_coder *coder, t_dongle *first, t_dongle *second)
{
	bool	both_acquired;

	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	both_acquired = (coder->id == first->owner && coder->id == second->owner);
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_unlock(&second->mutex);
	return (both_acquired);
}

int	acquire_dongles(t_coder *coder, t_scheduler sched)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->d_left;
	second = coder->d_right;
	if (first->id > second->id)
	{
		first = coder->d_right;
		second = coder->d_left;
	}
	while (!is_dongles_acquired(coder, first, second) && !is_stop(coder->table))
	{
		if (dongle_request(first, coder, sched))
			return (1);
		if (is_dongles_acquired(coder, first, second))
			break ;
		if (dongle_request(second, coder, sched))
			return (1);
	}
	if (is_stop(coder->table))
		return (1);
	return (0);
}

void	dongle_release(t_coder *coder)
{
	long	cooldown_ms;

	cooldown_ms = coder->table->config->dongle_cooldown;
	pthread_mutex_lock(&coder->d_left->mutex);
	if (coder->d_left->owner == coder->id)
	{
		coder->d_left->owner = 0;
		coder->d_left->state = COOLDOWN;
		coder->d_left->cooldown_end_ms = get_time_ms() + cooldown_ms;
	}
	pthread_cond_broadcast(&coder->d_left->cond);
	pthread_mutex_unlock(&coder->d_left->mutex);
	pthread_mutex_lock(&coder->d_right->mutex);
	if (coder->d_right->owner == coder->id)
	{
		coder->d_right->owner = 0;
		coder->d_right->state = COOLDOWN;
		coder->d_right->cooldown_end_ms = get_time_ms() + cooldown_ms;
	}
	pthread_cond_broadcast(&coder->d_right->cond);
	pthread_mutex_unlock(&coder->d_right->mutex);
}

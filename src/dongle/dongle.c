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

static bool	is_dongles_owned(t_coder *coder)
{
	bool	left_acquired;
	bool	right_acquired;

	if (coder->d_left == coder->d_right)
		return (false);
	pthread_mutex_lock(&coder->d_left->mutex);
	left_acquired = coder->d_left->owner == coder->id;
	pthread_mutex_unlock(&coder->d_left->mutex);
	pthread_mutex_lock(&coder->d_right->mutex);
	right_acquired = coder->d_right->owner == coder->id;
	pthread_mutex_unlock(&coder->d_right->mutex);
	return (left_acquired && right_acquired);
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
	while (!is_dongles_owned(coder) && !is_stop(coder->table))
	{
		if (dongle_request(first, coder, sched))
			return (1);
		if (is_dongles_owned(coder))
			break ;
		if (dongle_request(second, coder, sched))
			return (1);
	}
	return (0);
}

void	dongle_release(t_coder *coder)
{
	long	cooldown_ms;

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

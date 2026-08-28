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

int	acquire_dongles(t_coder *coder, t_scheduler sched)
{
	t_dongle	*first;
	t_dongle	*second;

	(void)sched;
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
	if (acquire_dongle(coder, first))
		return (1);
	if (acquire_dongle(coder, second))
		return (1);
	return (0);
}

void	dongle_release(t_coder *coder, t_dongle *dongle)
{
	long	key;
	t_table	*table;
	int		n_compiles;
	long	cooldown_ms;

	table = coder->table;
	pthread_mutex_lock(&coder->mutex);
	if (table->config->scheduler == EDF)
		key = coder->last_compile_time_ms + table->config->time_to_burnout;
	else
		key = get_time_ms();
	n_compiles = coder->compile_times + 1;
	pthread_mutex_unlock(&coder->mutex);
	cooldown_ms = coder->table->config->dongle_cooldown;
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner = 0;
	dongle->state = COOLDOWN;
	dongle->cooldown_end_ms = get_time_ms() + cooldown_ms;
	if (n_compiles < table->config->number_of_compiles_required)
		push_heap(dongle->heap, key, n_compiles, coder->id);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
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

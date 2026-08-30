/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:59 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:59 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder/coder.h"
#include "sys/time.h"
#include "table.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	sleep_coder_ms(t_coder *coder, long sleep_ms)
{
	struct timespec	sleep_time;
	long			now;

	now = get_time_ms();
	sleep_time.tv_sec = (now + sleep_ms) / 1000;
	sleep_time.tv_nsec = ((now + sleep_ms) % 1000) * 1e6;
	pthread_mutex_lock(&coder->mutex);
	while (!is_stop(coder->table))
	{
		if (pthread_cond_timedwait(&coder->cond, &coder->mutex,
				&sleep_time) != 0)
			break ;
	}
	pthread_mutex_unlock(&coder->mutex);
	return (is_stop(coder->table));
}

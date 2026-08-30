/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:38 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:38 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder/coder.h"
#include "log.h"

static void	report_dongle(t_coder *coder, t_dongle *dongle)
{
	long	time;
	long	acquire_time;

	time = coder->table->monitor->time_ms;
	pthread_mutex_lock(&dongle->mutex);
	acquire_time = dongle->acquire_time_ms - time;
	pthread_mutex_unlock(&dongle->mutex);
	printf("%ld %d has taken a dongle\n", acquire_time, coder->id);
}

static void	display_event(t_coder *coder, t_task task, long time, int id)
{
	if (task == COMPILING)
	{
		report_dongle(coder, coder->d_left);
		report_dongle(coder, coder->d_right);
		printf("%ld %d is compiling\n", time, id);
	}
	else if (task == DEBUGGING)
		printf("%ld %d is debugging\n", time, id);
	else if (task == REFACTORING)
		printf("%ld %d is refactoring\n", time, id);
}

void	*shutdown_simulation(int burned_coder, t_monitor *monitor)
{
	t_table	*table;
	long	time;

	table = monitor->table;
	set_stop(table);
	time = get_time_ms() - monitor->time_ms;
	printf("%ld %d burned out\n", time, burned_coder);
	return (NULL);
}

void	report_task(t_table *table, t_coder *coder)
{
	long	time;
	t_task	task;
	int		id;

	id = coder->id;
	task = coder->state;
	pthread_mutex_lock(&table->logger_mutex);
	time = get_time_ms() - table->monitor->time_ms;
	if (!is_stop(table))
		display_event(coder, task, time, id);
	pthread_mutex_unlock(&table->logger_mutex);
}

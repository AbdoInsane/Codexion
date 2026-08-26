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

static void	get_coder_data(t_coder *coder, int *id, t_task *task)
{
	pthread_mutex_lock(&coder->mutex);
	*task = coder->state;
	*id = coder->id;
	pthread_mutex_unlock(&coder->mutex);
}

static void	display_event(t_task task, long time, int id)
{
	if (task == COMPILING)
	{
		printf("%ld %d has taken a dongle\n", time, id);
		printf("%ld %d has taken a dongle\n", time, id);
		printf("%ld %d is compiling\n", time, id);
	}
	else if (task == DEBUGGING)
		printf("%ld %d is debugging\n", time, id);
	else if (task == REFACTORING)
		printf("%ld %d is refactoring\n", time, id);
	else if (task == BURNOUT)
		printf("%ld %d burned out\n", time, id);
}

void	report_task(t_table *table, t_coder *coder)
{
	long	time;
	t_task	task;
	int		id;

	if (is_stop(table))
		return ;
	time = get_time_ms() - table->monitor->time_ms;
	get_coder_data(coder, &id, &task);
	pthread_mutex_lock(&table->logger_mutex);
	display_event(task, time, id);
	pthread_mutex_unlock(&table->logger_mutex);
}

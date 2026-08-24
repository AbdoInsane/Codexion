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

void	report_task(t_table *table, int id, int task)
{
	long	time;

	time = get_time_ms() - table->monitor->time_ms;
	pthread_mutex_lock(&table->logger_mutex);
	if (task == COMPILING)
	{
		printf("%ld %d has taken a dongle\n", time, id);
		time = get_time_ms() - table->monitor->time_ms;
		printf("%ld %d has taken a dongle\n", time, id);
		time = get_time_ms() - table->monitor->time_ms;
		printf("%ld %d is compiling\n", time, id);
	}
	else if (task == DEBUGGING)
		printf("%ld %d is debugging\n", time, id);
	else if (task == REFACTORING)
		printf("%ld %d is refactoring\n", time, id);
	else if (task == BURNOUT)
		printf("%ld %d burned out\n", time, id);
	pthread_mutex_unlock(&table->logger_mutex);
}

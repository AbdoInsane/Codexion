/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:47 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:47 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "monitor.h"

int	monitor_start(t_table *table)
{
	if (pthread_create(&table->monitor->thread, NULL, monitor_routine,
			table->monitor))
		return (1);
	table->status.monitor_created = true;
	return (0);
}

t_monitor	*monitor_init(t_table *table)
{
	t_monitor	*monitor;

	monitor = ft_malloc(&table->memory, sizeof(t_monitor));
	if (!monitor)
		return (NULL);
	monitor->time_ms = 0;
	monitor->table = table;
	monitor->started_coders = 0;
	monitor->working_coders = table->config->number_of_coders;
	pthread_cond_init(&monitor->cond, NULL);
	pthread_mutex_init(&monitor->mutex, NULL);
	pthread_cond_init(&monitor->start_cond, NULL);
	return (monitor);
}

void	monitor_destroy(t_table *table)
{
	pthread_cond_destroy(&table->monitor->cond);
	pthread_mutex_destroy(&table->monitor->mutex);
	pthread_cond_destroy(&table->monitor->start_cond);
}

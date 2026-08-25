/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 20:32:11 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 20:32:11 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder/coder.h"
#include "logger/log.h"
#include "monitor.h"
#include <unistd.h>

static int	check_deadlines(t_table *table)
{
	long	deadline;
	long	last_compile;
	t_task	state;
	int		i;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		pthread_mutex_lock(&table->coders[i].mutex);
		state = table->coders[i].state;
		last_compile = table->coders[i].last_compile_time_ms;
		pthread_mutex_unlock(&table->coders[i].mutex);
		deadline = last_compile + table->config->time_to_burnout;
		if (deadline < get_time_ms() && state != FINISHED)
			return (i);
		i++;
	}
	return (-1);
}

static void	wait_for_coders(t_monitor *monitor)
{
	t_table	*table;
	int		i;

	table = monitor->table;
	pthread_mutex_lock(&monitor->mutex);
	while (monitor->started_coders < table->config->number_of_coders)
		pthread_cond_wait(&monitor->cond, &monitor->mutex);
	i = 0;
	monitor->time_ms = get_time_ms();
	while (i < table->config->number_of_coders)
	{
		pthread_mutex_lock(&table->coders[i].mutex);
		table->coders[i].last_compile_time_ms = monitor->time_ms;
		pthread_mutex_unlock(&table->coders[i].mutex);
		i++;
	}
	pthread_cond_broadcast(&monitor->start_cond);
	pthread_mutex_unlock(&monitor->mutex);
}

static bool	all_finished(t_monitor *monitor)
{
	bool	done;

	pthread_mutex_lock(&monitor->mutex);
	done = (monitor->working_coders == 0);
	pthread_mutex_unlock(&monitor->mutex);
	return (done);
}

static void	handle_burnout(t_table *table, int index)
{
	t_coder	*burned_coder;

	burned_coder = &table->coders[index];
	set_coder_task(burned_coder, BURNOUT);
	report_task(table, burned_coder);
}

void	*monitor_routine(void *arg)
{
	t_monitor	*self;
	int			exit_status;

	self = (t_monitor *)arg;
	wait_for_coders(self);
	while (!all_finished(self))
	{
		exit_status = check_deadlines(self->table);
		if (exit_status != -1)
		{
			handle_burnout(self->table, exit_status);
			return (NULL);
		}
		usleep(500);
	}
	set_stop(self->table);
	return (self);
}

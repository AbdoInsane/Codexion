/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:24:59 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 15:52:15 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

void	set_coder_task(t_coder *coder, t_task task)
{
	coder->state = task;
	if (task == COMPILING)
	{
		coder->compile_times++;
		coder->last_compile_time_ms = get_time_ms();
	}
}

static int	coder_task(t_coder *coder, t_task task)
{
	t_table	*table;
	long	task_time_ms;

	table = coder->table;
	if (task == COMPILING)
		task_time_ms = table->config->time_to_compile;
	else if (task == DEBUGGING)
		task_time_ms = table->config->time_to_debug;
	else if (task == REFACTORING)
		task_time_ms = table->config->time_to_refactor;
	pthread_mutex_lock(&coder->mutex);
	set_coder_task(coder, task);
	if (task == WAITING || task == FINISHED)
		return (pthread_mutex_unlock(&coder->mutex), 0);
	report_task(table, coder);
	pthread_mutex_unlock(&coder->mutex);
	if (sleep_coder_ms(coder, task_time_ms))
		return (1);
	return (0);
}

static void	coder_finish(t_coder *coder)
{
	t_monitor	*monitor;

	monitor = coder->table->monitor;
	coder_task(coder, FINISHED);
	pthread_mutex_lock(&monitor->mutex);
	monitor->working_coders--;
	pthread_cond_signal(&monitor->cond);
	pthread_mutex_unlock(&monitor->mutex);
}

static int	register_coders(t_coder *coder)
{
	t_monitor	*monitor;
	t_table		*table;

	table = coder->table;
	monitor = table->monitor;
	pthread_mutex_lock(&monitor->mutex);
	monitor->working_coders++;
	pthread_cond_signal(&monitor->cond);
	while (!monitor->simulation_started && !is_stop(table))
		pthread_cond_wait(&monitor->start_cond, &monitor->mutex);
	pthread_mutex_unlock(&monitor->mutex);
	return (is_stop(table));
}

void	*coder_routine(void *arg)
{
	t_coder	*self;
	t_table	*table;
	int		req_compiles;

	self = (t_coder *)arg;
	table = self->table;
	req_compiles = table->config->number_of_compiles_required;
	if (register_coders(self))
		return (coder_finish(self), NULL);
	while (self->compile_times < req_compiles)
	{
		if (acquire_dongles(self))
			break ;
		coder_task(self, COMPILING);
		release_dongle(self, self->d_left);
		release_dongle(self, self->d_right);
		if (coder_task(self, DEBUGGING))
			break ;
		if (coder_task(self, REFACTORING))
			break ;
		if (coder_task(self, WAITING))
			break ;
	}
	coder_finish(self);
	return (self);
}

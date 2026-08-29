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
		coder->last_compile_time_ms = get_time_ms();
}

static int	coder_task(t_coder *coder, t_task task)
{
	t_table	*table;
	long	task_time_ms;

	table = coder->table;
	if (task == WAITING || task == FINISHED)
		return (set_coder_task(coder, task), 0);
	else if (task == COMPILING)
		task_time_ms = table->config->time_to_compile;
	else if (task == DEBUGGING)
		task_time_ms = table->config->time_to_debug;
	else if (task == REFACTORING)
		task_time_ms = table->config->time_to_refactor;
	else
		return (1);
	pthread_mutex_lock(&coder->mutex);
	set_coder_task(coder, task);
	report_task(table, coder);
	if (wait_ms(table, &coder->mutex, &table->cond, task_time_ms))
		return (1);
	pthread_mutex_unlock(&coder->mutex);
	return (0);
}

static void	coder_finish(t_coder *coder)
{
	t_monitor	*monitor;

	monitor = coder->table->monitor;
	coder_task(coder, FINISHED);
	pthread_mutex_lock(&monitor->mutex);
	monitor->working_coders--;
	pthread_mutex_unlock(&monitor->mutex);
}

static void	register_coder(t_coder *coder)
{
	t_monitor	*monitor;
	t_table		*table;

	table = coder->table;
	monitor = table->monitor;
	pthread_mutex_lock(&monitor->mutex);
	monitor->started_coders++;
	pthread_cond_signal(&monitor->cond);
	pthread_cond_wait(&monitor->start_cond, &monitor->mutex);
	pthread_mutex_unlock(&monitor->mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*self;
	t_table	*table;

	self = (t_coder *)arg;
	register_coder(self);
	table = self->table;
	while (self->compile_times < table->config->number_of_compiles_required)
	{
		if (acquire_dongles(self, table->config->scheduler))
			return (NULL);
		coder_task(self, COMPILING);
		dongle_release(self, self->d_left);
		dongle_release(self, self->d_right);
		coder_task(self, DEBUGGING);
		coder_task(self, REFACTORING);
		coder_task(self, WAITING);
		self->compile_times++;
	}
	coder_finish(self);
	return (self);
}

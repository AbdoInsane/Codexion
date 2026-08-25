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
	pthread_mutex_lock(&coder->mutex);
	coder->state = task;
	if (task == COMPILING)
		coder->last_compile_time_ms = get_time_ms();
	pthread_mutex_unlock(&coder->mutex);
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
	set_coder_task(coder, task);
	report_task(table, coder);
	if (wait_ms(table, &table->mutex, &table->cond, task_time_ms))
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
	int		compiles;
	int		failed;

	self = (t_coder *)arg;
	register_coder(self);
	compiles = 0;
	while (compiles < self->table->config->number_of_compiles_required)
	{
		if (acquire_dongles(self, self->table->config->scheduler))
			break ;
		failed = coder_task(self, COMPILING);
		dongle_release(self);
		if (failed || coder_task(self, DEBUGGING) || coder_task(self,
				REFACTORING))
			break ;
		coder_task(self, WAITING);
		compiles++;
	}
	coder_finish(self);
	return (self);
}

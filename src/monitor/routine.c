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
		if (table->coders[i].compile_times == 0)
			last_compile = table->monitor->time_ms;
		else
			last_compile = table->coders[i].last_compile_time_ms;
		state = table->coders[i].state;
		deadline = last_compile + table->config->time_to_burnout;
		pthread_mutex_unlock(&table->coders[i].mutex);
		if (deadline < (long)get_time_ms() && state != FINISHED)
			return (table->coders[i].id);
		i++;
	}
	return (-1);
}

static void	start_simulation(t_monitor *monitor)
{
	int	required_coders;

	required_coders = monitor->table->config->number_of_coders;
	pthread_mutex_lock(&monitor->mutex);
	while (monitor->working_coders < required_coders)
		pthread_cond_wait(&monitor->cond, &monitor->mutex);
	monitor->time_ms = get_time_ms();
	monitor->simulation_started = true;
	pthread_cond_broadcast(&monitor->start_cond);
	pthread_mutex_unlock(&monitor->mutex);
}

void	*monitor_routine(void *arg)
{
	t_monitor	*self;
	int			burned_coder;
	bool		coders_finished;

	self = (t_monitor *)arg;
	start_simulation(self);
	while (1)
	{
		pthread_mutex_lock(&self->mutex);
		coders_finished = self->working_coders == 0;
		pthread_mutex_unlock(&self->mutex);
		if (coders_finished)
			break ;
		burned_coder = check_deadlines(self->table);
		if (burned_coder != -1)
			return (shutdown_simulation(burned_coder, self));
	}
	return (self);
}

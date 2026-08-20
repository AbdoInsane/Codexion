#include "coder/coder.h"
#include "logger/log.h"
#include "monitor.h"

int	check_deadlines(t_table *table)
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
		pthread_mutex_lock(&table->monitor->mutex);
		if (!last_compile)
			last_compile = table->monitor->time_ms;
		deadline = last_compile + table->config->time_to_burnout;
		pthread_mutex_unlock(&table->monitor->mutex);
		if (deadline < get_time_ms() && state != FINISHED)
			return (table->coders[i].id);
		i++;
	}
	return (-1);
}

void	*monitor_routine(void *arg)
{
	t_monitor	*self;
	int			exit_status;

	self = (t_monitor *)arg;
	while (1)
	{
		pthread_mutex_lock(&self->mutex);
		if (self->working_coders == 0)
		{
			pthread_mutex_unlock(&self->mutex);
			break ;
		}
		pthread_mutex_unlock(&self->mutex);
		exit_status = check_deadlines(self->table);
		if (exit_status != -1)
		{
			set_stop(self->table);
			report_task(self->table, exit_status, BURNOUT);
			return (NULL);
		}
	}
	set_stop(self->table);
	return (self);
}

int	monitor_start(t_table *table)
{
	table->monitor->time_ms = get_time_ms();
	if (pthread_create(&table->monitor->thread, NULL, monitor_routine,
			table->monitor))
		return (1);
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
	monitor->working_coders = table->config->number_of_coders;
	pthread_cond_init(&monitor->cond, NULL);
	pthread_mutex_init(&monitor->mutex, NULL);
	return (monitor);
}

void	monitor_destroy(t_table *table)
{
	pthread_join(table->monitor->thread, NULL);
	pthread_mutex_destroy(&table->monitor->mutex);
	pthread_cond_destroy(&table->monitor->cond);
}

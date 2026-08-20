#include "coder.h"
#include "logger/log.h"

void	*coder_routine(void *arg)
{
	t_coder		*self;
	t_table		*table;
	t_monitor	*monitor;
	int			compiles;

	compiles = 0;
	self = (t_coder *)arg;
	table = self->table;
	monitor = table->monitor;
	while (compiles < table->config->number_of_compiles_required)
	{
		if (acquire_dongles(self, table->config->scheduler))
			return (NULL);
		self->state = ACQUIRING;
		report_task(table, self->id, self->state);
		self->state = COMPILING;
		self->last_compile_time_ms = get_time_ms();
		report_task(table, self->id, self->state);
		if (sleep_or_stop(table, table->config->time_to_compile))
			return (NULL);
		dongle_release(self);
		self->state = DEBUGGING;
		report_task(table, self->id, self->state);
		if (sleep_or_stop(table, table->config->time_to_debug))
			return (NULL);
		self->state = REFACTORING;
		report_task(table, self->id, self->state);
		if (sleep_or_stop(table, table->config->time_to_refactor))
			return (NULL);
		self->state = WAITING;
		compiles++;
	}
	pthread_mutex_lock(&monitor->mutex);
	self->state = FINISHED;
	monitor->working_coders--;
	pthread_mutex_unlock(&monitor->mutex);
	return (self);
}

int	coder_start(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		if (pthread_create(&table->coders[i].thread, NULL, coder_routine,
				&table->coders[i]))
			return (1);
		i++;
	}
	return (0);
}

t_coder	*coder_init(t_table *table)
{
	t_coder	*coders;
	int		size;
	int		i;

	size = table->config->number_of_coders;
	coders = ft_malloc(&table->memory, sizeof(t_coder) * size);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < table->config->number_of_coders)
	{
		coders[i].id = i;
		coders[i].table = table;
		coders[i].state = WAITING;
		coders[i].last_compile_time_ms = 0;
		coders[i].d_left = &table->dongles[i];
		coders[i].d_right = &table->dongles[(i + 1) % size];
		pthread_cond_init(&coders[i].cond, NULL);
		i++;
	}
	return (coders);
}

void	coder_destroy(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->config->number_of_coders)
		pthread_join(table->coders[i++].thread, NULL);
}

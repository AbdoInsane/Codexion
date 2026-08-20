#include "coder.h"
#include "logger/log.h"

void	set_coder_task(t_coder *coder, t_task task)
{
	pthread_mutex_lock(&coder->mutex);
	coder->state = task;
	if (task == COMPILING)
		coder->last_compile_time_ms = get_time_ms();
	pthread_mutex_unlock(&coder->mutex);
}

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
		set_coder_task(self, COMPILING);
		report_task(table, self->id, self->state);
		if (wait_ms(table, &table->mutex, &table->cond,
				table->config->time_to_compile))
			return (NULL);
		dongle_release(self);
		set_coder_task(self, DEBUGGING);
		report_task(table, self->id, self->state);
		if (wait_ms(table, &table->mutex, &table->cond,
				table->config->time_to_debug))
			return (NULL);
		set_coder_task(self, REFACTORING);
		report_task(table, self->id, self->state);
		if (wait_ms(table, &table->mutex, &table->cond,
				table->config->time_to_refactor))
			return (NULL);
		set_coder_task(self, WAITING);
		compiles++;
	}
	set_coder_task(self, FINISHED);
	pthread_mutex_lock(&monitor->mutex);
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
		coders[i].id = i + 1;
		coders[i].table = table;
		coders[i].state = WAITING;
		coders[i].last_compile_time_ms = 0;
		coders[i].d_left = &table->dongles[i];
		coders[i].d_right = &table->dongles[(i + 1) % size];
		pthread_mutex_init(&coders[i].mutex, NULL);
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

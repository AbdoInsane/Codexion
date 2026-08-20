#include "coder/coder.h"
#include "dongle/dongle.h"
#include "memory/memory.h"
#include "monitor/monitor.h"
#include "parser/parser.h"
#include "table.h"
#include <pthread.h>

bool	is_stop(t_table *table)
{
	bool	stoped;

	pthread_mutex_lock(&table->mutex);
	stoped = table->stop;
	pthread_mutex_unlock(&table->mutex);
	return (stoped);
}

void	set_stop(t_table *table)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&table->mutex);
	table->stop = true;
	pthread_mutex_unlock(&table->mutex);
	pthread_cond_broadcast(&table->cond);
	while (i < table->config->number_of_coders)
		pthread_cond_broadcast(&table->dongles[i++].cond);
}

int	table_start(t_table *table)
{
	if (monitor_start(table))
		return (1);
	if (coder_start(table))
		return (1);
	return (0);
}

t_table	*table_init(int argc, char **argv)
{
	t_table		*table;
	t_memory	*memory;

	memory = NULL;
	table = ft_malloc(&memory, sizeof(t_table));
	if (!table)
		return (NULL);
	table->stop = false;
	table->memory = memory;
	table->config = get_config(argc, argv, &table->memory);
	if (!table->config)
		return (ft_free(&table->memory), NULL);
	table->dongles = dongle_init(table);
	if (!table->dongles)
		return (ft_free(&table->memory), NULL);
	table->coders = coder_init(table);
	if (!table->coders)
		return (ft_free(&table->memory), NULL);
	table->monitor = monitor_init(table);
	if (!table->monitor)
		return (ft_free(&table->memory), NULL);
	pthread_mutex_init(&table->mutex, NULL);
	pthread_cond_init(&table->cond, NULL);
	pthread_mutex_init(&table->logger_mutex, NULL);
	return (table);
}

void	table_destroy(t_table *table)
{
	monitor_destroy(table);
	coder_destroy(table);
	dongle_destroy(table);
	pthread_cond_destroy(&table->cond);
	pthread_mutex_destroy(&table->mutex);
	ft_free(&table->memory);
}

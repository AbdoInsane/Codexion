#include "monitor.h"

// init monitor resources
t_monitor	*monitor_init(t_contex *contex)
{
	t_monitor	*monitor;

	monitor = ft_malloc(&contex->memory, sizeof(t_monitor));
	if (!monitor)
		return (NULL);
	monitor->time_ms = 0;
	monitor->working_coders = 0;
	pthread_cond_init(&monitor->cond, NULL);
	pthread_mutex_init(&monitor->mutex, NULL);
	return (monitor);
}

void	monitor_start(void);   // start monitor and simulation
void	monitor_destory(void); // release monitor resources

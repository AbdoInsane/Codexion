#include "monitor/monitor.h"
#include "scheduler.h"

// init scheduler resources
t_scheduler	*scheduler_init(t_contex *contex)
{
	t_scheduler	*scheduler;

	scheduler = ft_malloc(&contex->memory, sizeof(t_scheduler));
	if (!scheduler)
		return (NULL);
	scheduler->turn = -1;
	scheduler->heap = ft_malloc(&contex->memory, sizeof(t_heap));
	if (!scheduler->heap)
		return (NULL);
	scheduler->heap->capacity = contex->config->number_of_coders;
	scheduler->heap->size = 0;
	scheduler->heap->orders = ft_malloc(&contex->memory, sizeof(t_order)
			* contex->config->number_of_coders);
	if (!scheduler->heap->orders)
		return (NULL);
	pthread_cond_init(&scheduler->cond, NULL);
	pthread_mutex_init(&scheduler->mutex, NULL);
	return (scheduler);
}

// initialize full system components and start the whole system
int	scheduler_boot(t_contex *contex)
{
	contex->scheduler = scheduler_init(contex);
	if (!contex->scheduler)
		return (-1);
	contex->dongles = dongle_init(contex);
	if (!contex->dongles)
		return (-1);
	contex->coders = coder_init(contex);
	if (!contex->coders)
		return (-1);
	contex->monitor = monitor_init(contex);
	if (!contex->monitor)
		return (-1);
	return (0);
}

void	scheduler_start(void);   // start scheduler and simulation
void	scheduler_destroy(void); // release scheduler resources

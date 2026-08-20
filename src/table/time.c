#include "sys/time.h"
#include "table.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	wait_ms(t_table *table, pthread_mutex_t *mutex, pthread_cond_t *cond,
		long ms)
{
	struct timespec	deadline;
	long			now;
	int				exit_status;

	now = get_time_ms();
	deadline.tv_sec = (now + ms) / 1000;
	deadline.tv_nsec = ((now + ms) % 1000) * 1000000;
	while (!is_stop(table))
	{
		pthread_mutex_lock(mutex);
		exit_status = pthread_cond_timedwait(cond, mutex, &deadline);
		pthread_mutex_unlock(mutex);
		if (exit_status != 0)
			break ;
	}
	return (is_stop(table));
}

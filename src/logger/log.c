#include "coder/coder.h"
#include "log.h"

void	report_task(t_table *table, int id, int task)
{
	long	time;

	time = get_time_ms() - table->monitor->time_ms;
	pthread_mutex_lock(&table->logger_mutex);
	if (task == ACQUIRING)
	{
		printf(GREEN "%ld %d has taken a dongle\n" RESET, time, id);
		printf(GREEN "%ld %d has taken a dongle\n" RESET, time, id);
	}
	else if (task == COMPILING)
		printf(GREEN "%ld %d is compiling\n" RESET, time, id);
	else if (task == DEBUGGING)
		printf(GREEN "%ld %d is debugging\n" RESET, time, id);
	else if (task == REFACTORING)
		printf(GREEN "%ld %d is refactoring\n" RESET, time, id);
	else if (task == BURNOUT)
		printf(RED "%ld %d burned out\n" RESET, time, id);
	pthread_mutex_unlock(&table->logger_mutex);
}

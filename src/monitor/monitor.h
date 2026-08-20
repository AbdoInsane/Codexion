#ifndef MONITOR_H
# define MONITOR_H

# include "memory/memory.h"
# include "parser/parser.h"
# include "table/table.h"
# include <pthread.h>

typedef struct s_monitor
{
	pthread_t		thread;
	long			time_ms;
	pthread_cond_t	cond;
	pthread_mutex_t	mutex;
	int				working_coders;

	t_table			*table;
}					t_monitor;

t_monitor			*monitor_init(t_table *table);
int					monitor_start(t_table *table);
void				monitor_destroy(t_table *table);

#endif

#ifndef MONITOR_H
# define MONITOR_H

# include "contex.h"
# include <bits/pthreadtypes.h>
# include <pthread.h>

typedef struct s_monitor
{
	pthread_t		thread;
	unsigned int	time_ms;
	pthread_cond_t	cond;
	pthread_mutex_t	mutex;
	int				working_coders;
}					t_monitor;

t_monitor			*monitor_init(t_contex *contex);
void				monitor_start(void);
void				monitor_destory(void);

#endif

#ifndef SCHEDULER_H
# define SCHEDULER_H

# include "coder/coder.h"
# include "contex.h"
# include "dongle/dongle.h"
# include "heap.h"
# include "memory/memory.h"
# include "parser/parser.h"
# include <pthread.h>

typedef struct s_scheduler
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	pthread_t		thread;
	t_heap			*heap;
	int				turn;
}					t_scheduler;

int					scheduler_boot(t_contex *contex);
t_scheduler			*scheduler_init(t_contex *contex);
void				scheduler_start(void);
void				scheduler_destroy(void);

#endif

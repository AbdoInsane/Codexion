#ifndef DONGLE_H
# define DONGLE_H

# include "contex.h"
# include "parser/parser.h"
# include <pthread.h>

typedef enum s_state
{
	FREE,
	COOLDOWN,
	ACQUIRED,
}					t_state;

typedef struct s_dongle
{
	int				id;
	t_state			state;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
}					t_dongle;

t_dongle			*dongle_init(t_contex *contex);
void				dongle_acquire(void);
void				dongle_release(void);
void				dongle_destroy(void);

#endif

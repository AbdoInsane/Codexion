#ifndef CODER_H
# define CODER_H

# include "contex.h"
# include "dongle/dongle.h"
# include "parser/parser.h"
# include <pthread.h>

typedef enum e_task
{
	WAITING,
	COMPILING,
	DEBUGGING,
	REFACTORING
}					t_task;

typedef struct s_coder
{
	int				id;
	t_task			state;
	pthread_t		thread;
	t_dongle		*d_left;
	t_dongle		*d_right;
	unsigned int	last_compile_time_ms;
}					t_coder;

t_coder				*coder_init(t_contex *contex);
void				coder_start(void);
void				coder_wait(void);
void				coder_destroy(void);
#endif

#ifndef CONTEX_H
# define CONTEX_H

# include "memory/memory.h"
# include <stdbool.h>

typedef struct s_coder		t_coder;
typedef struct s_config		t_config;
typedef struct s_dongle		t_dongle;
typedef struct s_memory		t_memory;
typedef struct s_monitor	t_monitor;
typedef struct s_scheduler	t_scheduler;

typedef struct s_contex
{
	t_config				*config;
	t_scheduler				*scheduler;
	t_monitor				*monitor;

	t_memory				*memory;

	t_coder					*coders;
	t_dongle				*dongles;

	bool					stop;
}							t_contex;

#endif

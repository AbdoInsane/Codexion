#ifndef PARSER_H
# define PARSER_H

# include "memory/memory.h"
# include <string.h>

typedef enum t_scheduler
{
	FIFO,
	EDF,
}				t_scheduler;

typedef struct s_config
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}				t_config;

t_config		*get_config(int argc, char **argv, t_memory **memory);
char			*ft_strdup(char *str, t_memory **memory);
int				ft_isdigit(const char *str);
#endif

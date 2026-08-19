#include "coder.h"

// inititalize coder and allocate resouces
t_coder	*coder_init(t_contex *contex)
{
	t_coder	*coders;
	int		size;
	int		i;

	size = contex->config->number_of_coders;
	coders = ft_malloc(&contex->memory, sizeof(t_coder) * size);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < contex->config->number_of_coders)
	{
		coders[i].id = i;
		coders[i].state = WAITING;
		coders[i].last_compile_time_ms = 0;
		coders[i].d_left = &contex->dongles[i];
		coders[i].d_right = &contex->dongles[(i + 1) % size];
		i++;
	}
	return (coders);
}

void	coder_start(void);   // start coding life cycle
void	coder_wait(void);    // sleep coder
void	coder_destroy(void); // release coder resources

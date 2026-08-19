#include "dongle.h"

// init dongle to each coder
t_dongle	*dongle_init(t_contex *contex)
{
	t_dongle	*dongles;
	int			size;
	int			i;

	size = contex->config->number_of_coders;
	dongles = ft_malloc(&contex->memory, sizeof(t_dongle) * size);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < size)
	{
		dongles[i].id = i;
		dongles[i].state = FREE;
		pthread_mutex_init(&dongles[i].mutex, NULL);
		pthread_cond_init(&dongles[i].cond, NULL);
		i++;
	}
	return (dongles);
}

void	dongle_acquire(void); // acquire dongle to a coder
void	dongle_release(void); // release dongle from a coder
void	dongle_destroy(void); // destroy dongle resouces

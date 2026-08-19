#ifndef MEMORY_H
# define MEMORY_H

# include <stdlib.h>

typedef struct s_memory
{
	void			*data;
	struct s_memory	*next;
}					t_memory;

void				*ft_malloc(t_memory **head, size_t size);
int					ft_free(t_memory **head);

#endif

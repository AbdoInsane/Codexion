#ifndef HEAP_H
# define HEAP_H

typedef struct s_order
{
	long	key;
	int		id;
}			t_order;

typedef struct s_heap
{
	int		capacity;
	int		size;
	t_order	*orders;
}			t_heap;

void		push_heap(t_heap *heap, long key, int id);
int			pop_heap(t_heap *heap);

#endif

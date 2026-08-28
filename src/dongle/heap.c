/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 18:31:18 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 18:31:18 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

static bool	is_less(t_heap *heap, int a, int b)
{
	if (!heap)
		return (false);
	if (a < 0 || a >= heap->size)
		return (false);
	if (b < 0 || b >= heap->size)
		return (false);
	if (heap->orders[a].key != heap->orders[b].key)
		return (heap->orders[a].key < heap->orders[b].key);
	if (heap->orders[a].n_compiles != heap->orders[b].n_compiles)
		return (heap->orders[a].n_compiles < heap->orders[b].n_compiles);
	if ((heap->orders[a].id + heap->orders[b].id) % 2)
		return (heap->orders[a].id % 2);
	return (heap->orders[a].id < heap->orders[b].id);
}

static void	heapify_up(t_heap *heap)
{
	t_order	tmp;
	int		i;
	int		p;

	i = heap->size - 1;
	p = (i - 1) / 2;
	while (i >= 0 && is_less(heap, i, p))
	{
		tmp = heap->orders[p];
		heap->orders[p] = heap->orders[i];
		heap->orders[i] = tmp;
		i = p;
		p = (i - 1) / 2;
	}
}

static void	heapify_down(t_heap *heap)
{
	int		i;
	int		smallest;
	t_order	tmp;

	i = 0;
	while (1)
	{
		smallest = i;
		if ((i * 2) + 1 < heap->size && is_less(heap, (i * 2) + 1, smallest))
			smallest = (i * 2) + 1;
		if ((i * 2) + 2 < heap->size && is_less(heap, (i * 2) + 2, smallest))
			smallest = (i * 2) + 2;
		if (i == smallest)
			break ;
		tmp = heap->orders[smallest];
		heap->orders[smallest] = heap->orders[i];
		heap->orders[i] = tmp;
		i = smallest;
	}
}

void	push_heap(t_heap *heap, long key, int n_compiles, int id)
{
	if (!heap || heap->size >= heap->capacity)
		return ;
	heap->orders[heap->size] = (t_order){key, n_compiles, id};
	heap->size++;
	heapify_up(heap);
}

int	pop_heap(t_heap *heap)
{
	int	id;

	if (!heap || heap->size == 0)
		return (-1);
	id = heap->orders[0].id;
	heap->orders[0] = heap->orders[heap->size - 1];
	heap->size--;
	heapify_down(heap);
	return (id);
}

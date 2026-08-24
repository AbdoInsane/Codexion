/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:34 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:34 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEAP_H
# define HEAP_H

# include <stdbool.h>

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

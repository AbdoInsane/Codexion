/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:32 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:32 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DONGLE_H
# define DONGLE_H

# include "heap.h"
# include "logger/log.h"
# include "parser/parser.h"
# include "table/table.h"
# include <pthread.h>
# include <unistd.h>

# define HEAP_SIZE 2

typedef enum s_state
{
	FREE,
	COOLDOWN,
	ACQUIRED,
}					t_state;

typedef struct s_dongle
{
	int				id;
	pthread_cond_t	cond;
	pthread_mutex_t	mutex;
	int				owner;
	t_state			state;
	t_heap			*heap;
	long			acquire_time_ms;
	long			cooldown_end_ms;
}					t_dongle;

int					dongle_request(t_dongle *dongle, t_coder *coder,
						t_scheduler scheduler);

int					acquire_dongle(t_coder *coder, t_dongle *dongle);
t_dongle			*init_dongles(t_table *table);
int					acquire_dongles(t_coder *coder);
void				release_dongle(t_coder *coder, t_dongle *dongle);
void				destroy_dongles(t_table *table);

void				push_order(t_coder *coder, t_dongle *dongle);

#endif

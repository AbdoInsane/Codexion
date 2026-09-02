/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:46 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:46 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MONITOR_H
# define MONITOR_H

# include "memory/memory.h"
# include "parser/parser.h"
# include "table/table.h"
# include <pthread.h>

typedef struct s_monitor
{
	long			time_ms;
	bool			in_work;
	int				working_coders;
	int				started_coders;
	bool			simulation_started;
	pthread_cond_t	start_cond;
	pthread_t		thread;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;

	t_table			*table;
}					t_monitor;

t_monitor			*monitor_init(t_table *table);
int					monitor_start(t_table *table);
void				*monitor_routine(void *arg);
void				monitor_destroy(t_table *table);

#endif

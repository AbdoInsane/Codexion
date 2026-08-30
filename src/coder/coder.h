/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:28 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:28 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

# include "dongle/dongle.h"
# include "monitor/monitor.h"
# include "parser/parser.h"
# include "table/table.h"
# include <pthread.h>

typedef enum e_task
{
	WAITING,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	FINISHED,
}					t_task;

typedef struct s_coder
{
	int				id;
	pthread_cond_t	cond;
	pthread_mutex_t	mutex;
	t_task			state;
	pthread_t		thread;
	t_dongle		*d_left;
	t_dongle		*d_right;
	int				compile_times;
	long			last_compile_time_ms;

	t_table			*table;
}					t_coder;

void				set_coder_task(t_coder *coder, t_task task);
int					coder_start(t_table *table);
t_coder				*coder_init(t_table *table);
void				*coder_routine(void *arg);
void				coder_destroy(t_table *table);

#endif

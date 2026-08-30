/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:57 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:57 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TABLE_H
# define TABLE_H

# include <pthread.h>
# include <stdbool.h>

typedef struct s_coder		t_coder;
typedef struct s_config		t_config;
typedef struct s_dongle		t_dongle;
typedef struct s_memory		t_memory;
typedef struct s_monitor	t_monitor;

typedef struct s_table_status
{
	bool					monitor_created;
	int						coders_created;
}							t_table_status;

typedef struct s_table
{
	t_config				*config;
	t_memory				*memory;

	t_monitor				*monitor;
	t_coder					*coders;
	t_dongle				*dongles;

	t_table_status			status;

	bool					stop;

	pthread_cond_t			cond;
	pthread_mutex_t			mutex;
	pthread_mutex_t			logger_mutex;
}							t_table;

t_table						*table_init(int argc, char **argv);
int							table_start(t_table *table);
void						table_destroy(t_table *table);
void						stop_simulation(t_table *table);
bool						is_stop(t_table *table);

long						get_time_ms(void);
int							sleep_coder_ms(t_coder *coder, long sleep_ms);

#endif

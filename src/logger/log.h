/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:39 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:39 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_H
# define LOG_H

# include "monitor/monitor.h"
# include "table/table.h"
# include <stdio.h>

void	*shutdown_simulation(int burned_coder, t_monitor *monitor);
void	report_dongle(t_coder *coder, t_dongle *dongle);
void	report_task(t_table *table, t_coder *coder);

#endif

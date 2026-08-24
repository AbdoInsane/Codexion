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

void	report_task(t_table *table, int id, int task);

#endif

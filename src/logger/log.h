#ifndef LOG_H
# define LOG_H

# include "monitor/monitor.h"
# include "table/table.h"
# include <stdio.h>

# define RESET "\033[0m"
# define RED "\033[31m"
# define GREEN "\033[32m"

void	report_task(t_table *table, int id, int task);

#endif

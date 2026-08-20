#include "table/table.h"

int	main(int argc, char **argv)
{
	t_table	*table;

	table = table_init(argc, argv);
	if (!table)
		return (0);
	table_start(table);
	table_destroy(table);
	return (0);
}

#include "contex.h"
#include "memory/memory.h"
#include "parser/parser.h"
#include "scheduler/scheduler.h"

int	main(int argc, char **argv)
{
	t_contex	contex;

	contex = (t_contex){0};
	contex.config = get_config(argc, argv, &contex.memory);
	if (!contex.config)
		return (ft_free(&contex.memory));
	scheduler_boot(&contex);
	ft_free(&contex.memory);
	return (0);
}

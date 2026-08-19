#include "parser.h"

// Parse command line arguments, allocate memory for the arguments
static void	*get_args(int argc, char **argv, t_memory **memory)
{
	char	**args;
	int		arg_i;

	args = NULL;
	arg_i = 0;
	if (argc != 9)
		return (NULL);
	args = ft_malloc(memory, sizeof(void *) * 8);
	if (!args)
		return (NULL);
	while (arg_i < 8)
	{
		args[arg_i] = (char *)ft_strdup(argv[arg_i + 1], memory);
		arg_i++;
	}
	return (args);
}

// Validate the numeric arguments gt 0 and ge 0 in the sixth argument
static int	validate_number(char *val, int arg_i)
{
	int	num;

	if (!val || !ft_isdigit(val))
		return (-1);
	num = atoi(val);
	if (num < 0)
		return (-1);
	if (arg_i != 6 && num == 0)
		return (-1);
	return (num);
}

// Validate the scheduler argument, only 'fifo' and 'edf' are valid
static int	validate_scheduler(char *val)
{
	if (!val)
		return (-1);
	if (strcmp(val, "fifo") == 0)
		return (FIFO);
	if (strcmp(val, "edf") == 0)
		return (EDF);
	return (-1);
}

// The main function to parse command line arguments and validate them
// Returns 't_args' struct with validated arguments, or NULL on failure
t_config	*get_config(int argc, char **argv, t_memory **memory)
{
	t_config	*valid_args;
	int			*p;
	void		**args;
	int			arg_i;

	args = get_args(argc, argv, memory);
	if (!args)
		return (NULL);
	valid_args = ft_malloc(memory, sizeof(t_config));
	if (!valid_args)
		return (NULL);
	p = (int *)valid_args;
	arg_i = 0;
	while (arg_i < 8)
	{
		if (arg_i < 7)
			p[arg_i] = (int)validate_number((char *)args[arg_i], arg_i);
		else
			p[arg_i] = validate_scheduler(args[arg_i]);
		if (p[arg_i] == -1)
			return (NULL);
		arg_i++;
	}
	return (valid_args);
}

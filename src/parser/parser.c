/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:52 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:52 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

// Parse command line arguments, allocate memory for the arguments
static void	*get_args(int argc, char **argv, t_memory **memory)
{
	char	**args;
	int		arg_i;

	args = NULL;
	arg_i = 0;
	if (argc != 9)
	{
		fprintf(stderr,
			RED "Error: Invalid number of arguments\n" YEL "Arguments order:\n"
			"1.\tnumber_of_coder\n2.\ttime_to_burnout\n3.\ttime_to_compile\n"
			"4.\ttime_to_debug\n5.\ttime_to_refactor\n6.\tnumber_of_compiles\n"
			"7.\tcooldown_time\n8.\tscheduler(fifo/edf)\n" RESET);
		return (NULL);
	}
	args = ft_malloc(memory, sizeof(char *) * 8);
	if (!args)
		return (NULL);
	while (arg_i < 8)
	{
		args[arg_i] = (char *)ft_strdup(argv[arg_i + 1], memory);
		if (!args[arg_i])
			return (NULL);
		arg_i++;
	}
	return (args);
}

// Validate the numeric arguments gt 0 and ge 0 in the sixth argument
static int	validate_number(char *val, int arg_i)
{
	int	num;

	if (!val || !ft_isnumber(val))
	{
		fprintf(stderr, RED "Error: Argument %d is not a valid number\n" RESET,
			arg_i + 1);
		return (-1);
	}
	num = atoi(val);
	if (num < 0)
	{
		fprintf(stderr, RED "Error: Argument %d must be positive\n" RESET, arg_i
			+ 1);
		return (-1);
	}
	if (arg_i != 6 && num == 0)
	{
		fprintf(stderr,
			RED "Error: Argument %d must be "
			"greater then zero\n" RESET,
			arg_i + 1);
		return (-1);
	}
	return (num);
}

// Validate the scheduler argument, only 'fifo' and 'edf' are valid
static int	validate_scheduler(char *val)
{
	if (!val)
	{
		fprintf(stderr, RED "Error: Invalid Scheduler\n" RESET);
		return (-1);
	}
	if (strcmp(val, "fifo") == 0)
		return (FIFO);
	if (strcmp(val, "edf") == 0)
		return (EDF);
	fprintf(stderr,
		RED "Error: Invalid scheduler\n" YEL "USE: fifo/edf\n" RESET);
	return (-1);
}

// The main function to parse command line arguments and validate them
// Returns 't_args' struct with validated arguments, or NULL on failure
t_config	*get_config(int argc, char **argv, t_memory **memory)
{
	t_config	*valid_args;
	int			*ptr;
	void		**args;
	int			arg_i;

	args = get_args(argc, argv, memory);
	if (!args)
		return (NULL);
	valid_args = ft_malloc(memory, sizeof(t_config));
	if (!valid_args)
		return (NULL);
	arg_i = 0;
	ptr = (int *)valid_args;
	while (arg_i < 8)
	{
		if (arg_i == 7)
			ptr[arg_i] = validate_scheduler(args[arg_i]);
		else
			ptr[arg_i] = (int)validate_number((char *)args[arg_i], arg_i);
		if (ptr[arg_i] == -1)
			return (NULL);
		arg_i++;
	}
	return (valid_args);
}

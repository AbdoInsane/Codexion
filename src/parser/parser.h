/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:54 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:54 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "memory/memory.h"
# include <stdio.h>
# include <string.h>

# define GRN "\e[0;32m"
# define RED "\e[0;31m"
# define YEL "\e[0;33m"
# define RESET "\e[0m"

typedef enum t_scheduler
{
	FIFO,
	LIFO,
	EDF,
}				t_scheduler;

typedef struct s_config
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}				t_config;

t_config		*get_config(int argc, char **argv, t_memory **memory);
char			*ft_strdup(char *str, t_memory **memory);
int				ft_isnumber(const char *str);
#endif

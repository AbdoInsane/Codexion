/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:12 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:12 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

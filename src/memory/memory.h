/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abait-mo <abait-mo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 08:11:44 by abait-mo          #+#    #+#             */
/*   Updated: 2026/08/24 08:11:44 by abait-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
# define MEMORY_H

# include <stdlib.h>

typedef struct s_memory
{
	void			*data;
	struct s_memory	*next;
}					t_memory;

void				*ft_malloc(t_memory **memory, size_t size);
int					ft_free(t_memory **memory);

#endif

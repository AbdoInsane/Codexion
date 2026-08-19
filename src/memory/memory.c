#include "memory.h"

// Adds a new allocation to the collector's linked list
static int	ft_collecte(t_memory **head, void *data)
{
	t_memory	*new;

	new = (t_memory *)malloc(sizeof(t_memory));
	if (!new)
		return (1);
	new->data = data;
	new->next = *head;
	*head = new;
	return (0);
}

// Replaces the standard malloc function with a custom collector-based version
// Allocates memory and adds it to the collector's linked list
void	*ft_malloc(t_memory **head, size_t size)
{
	void	*mem;

	if (size == 0)
		return (NULL);
	mem = (void *)malloc(size);
	if (!mem)
		return (NULL);
	if (ft_collecte(head, mem))
	{
		free(mem);
		return (NULL);
	}
	return (mem);
}

// Frees all memory allocated by the collector's linked list
int	ft_free(t_memory **head)
{
	t_memory	*current;
	t_memory	*prev;

	if (!head || !*head)
		return (-1);
	current = *head;
	prev = NULL;
	while (current)
	{
		prev = current;
		current = current->next;
		free(prev->data);
		free(prev);
	}
	return (-1);
}

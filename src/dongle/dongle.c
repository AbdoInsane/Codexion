#include "coder/coder.h"
#include "dongle.h"

static int	wait_for_dongl(t_coder *coder, t_dongle *dongle)
{
	t_order	*turn;

	turn = &dongle->heap->orders[0];
	while ((turn->id != coder->id || dongle->state == ACQUIRED)
		&& !is_stop(coder->table))
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	if (is_stop(coder->table))
		return (1);
	while (get_time_ms() < dongle->cooldown_end_ms && dongle->state == COOLDOWN)
	{
		pthread_mutex_unlock(&dongle->mutex);
		if (wait_ms(coder->table, &dongle->mutex, &dongle->cond,
				dongle->cooldown_end_ms - get_time_ms()))
		{
			pthread_mutex_lock(&dongle->mutex);
			break ;
		}
		pthread_mutex_lock(&dongle->mutex);
	}
	if (is_stop(coder->table))
		return (1);
	return (0);
}

static int	dongle_request(t_dongle *dongle, t_coder *coder,
		t_scheduler scheduler)
{
	long	key;

	if (scheduler == EDF)
		key = coder->last_compile_time_ms
			+ coder->table->config->time_to_burnout;
	else
		key = get_time_ms();
	pthread_mutex_lock(&dongle->mutex);
	push_heap(dongle->heap, key, coder->id);
	if (wait_for_dongl(coder, dongle))
		return (pthread_mutex_unlock(&dongle->mutex), 1);
	pop_heap(dongle->heap);
	dongle->state = ACQUIRED;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

int	acquire_dongles(t_coder *coder, t_scheduler sched)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->d_left->id < coder->d_right->id)
	{
		first = coder->d_left;
		second = coder->d_right;
	}
	else
	{
		first = coder->d_right;
		second = coder->d_left;
	}
	if (dongle_request(first, coder, sched))
		return (1);
	if (dongle_request(second, coder, sched))
		return (1);
	return (0);
}

void	dongle_release(t_coder *coder)
{
	pthread_mutex_lock(&coder->d_left->mutex);
	coder->d_left->state = COOLDOWN;
	coder->d_left->cooldown_end_ms = get_time_ms()
		+ coder->table->config->dongle_cooldown;
	pthread_mutex_unlock(&coder->d_left->mutex);
	pthread_cond_broadcast(&coder->d_left->cond);
	pthread_mutex_lock(&coder->d_right->mutex);
	coder->d_right->state = COOLDOWN;
	coder->d_right->cooldown_end_ms = get_time_ms()
		+ coder->table->config->dongle_cooldown;
	pthread_mutex_unlock(&coder->d_right->mutex);
	pthread_cond_broadcast(&coder->d_right->cond);
}

void	dongle_destroy(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->config->number_of_coders)
	{
		pthread_cond_destroy(&table->dongles[i].cond);
		pthread_mutex_destroy(&table->dongles[i].mutex);
		i++;
	}
}

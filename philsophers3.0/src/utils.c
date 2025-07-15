#include "philosophers.h"

long long	current_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	interruptible_sleep(int ms, t_philo *philo)
{
	long long	start;

	start = current_timestamp_ms();
	while (philo->running_sim && current_timestamp_ms() - start < ms)
		usleep(50);
}

void	print_state(t_philo *philo, char *state)
{
	pthread_mutex_lock(philo->print_mutex);
	if (philo->running_sim || (!philo->running_sim && philo->params->num_of_philos != 1))
		printf("%lld %d %s\n", current_timestamp_ms() - philo->params->start_time, philo->id, state);
	pthread_mutex_unlock(philo->print_mutex);
}
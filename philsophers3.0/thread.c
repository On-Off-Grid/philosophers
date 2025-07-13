#include "philosophers.h"

static void	*single_philo_routine(t_philo *philo)
{
	pthread_mutex_lock(&philo->left_fork->mutex);
	print_state(philo, "has taken a fork");
	interruptible_sleep(philo->params->time_to_die, philo);
	print_state(philo, "died");
	pthread_mutex_unlock(&philo->left_fork->mutex);
	return (NULL);
}

static void	get_forks(t_philo *philo, t_fork **first, t_fork **second)
{
	if (philo->id % 2 == 1)
	{
		*first = philo->left_fork;
		*second = philo->right_fork;
	}
	else
	{
		*first = philo->right_fork;
		*second = philo->left_fork;
	}
}

static void	eat_routine(t_philo *philo)
{
	pthread_mutex_lock(philo->time_mutex);
	philo->last_meal_time = current_timestamp_ms();
	pthread_mutex_unlock(philo->time_mutex);
	philo->state = EATING;
	print_state(philo, "is eating");
	interruptible_sleep(philo->params->time_to_eat, philo);
	philo->meals_eaten++;
}

static int	take_forks(t_philo *philo)
{
	t_fork	*first_fork;
	t_fork	*second_fork;

	get_forks(philo, &first_fork, &second_fork);
	pthread_mutex_lock(&first_fork->mutex);
	if (!philo->running_sim)
	{
		pthread_mutex_unlock(&first_fork->mutex);
		return (0);
	}
	print_state(philo, "has taken a fork");
	pthread_mutex_lock(&second_fork->mutex);
	if (!philo->running_sim)
	{
		pthread_mutex_unlock(&second_fork->mutex);
		pthread_mutex_unlock(&first_fork->mutex);
		return (0);
	}
	print_state(philo, "has taken a fork");
	eat_routine(philo);
	pthread_mutex_unlock(&second_fork->mutex);
	pthread_mutex_unlock(&first_fork->mutex);
	return (1);
}

void	*thread_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->params->num_of_philos == 1)
		return (single_philo_routine(philo));
	if (philo->id % 2 == 0)
		usleep(100);
	while (philo->running_sim)
	{
		if (!take_forks(philo) || !philo->running_sim)
			break ;
		philo->state = SLEEPING;
		print_state(philo, "is sleeping");
		interruptible_sleep(philo->params->time_to_sleep, philo);
		if (!philo->running_sim)
			break ;
		philo->state = THINKING;
		print_state(philo, "is thinking");
		usleep(100);
	}
	return (NULL);
}
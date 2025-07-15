#include "philosophers.h"

static int	check_meals_completion(t_sim *sim)
{
	int	i;
	int	all_done;

	if (sim->param.must_eat_time <= 0)
		return (0);
	all_done = 1;
	i = 0;
	while (i < sim->param.num_of_philos)
	{
		if (sim->philos[i].meals_eaten < sim->param.must_eat_time)
		{
			all_done = 0;
			break ;
		}
		i++;
	}
	return (all_done);
}

static void	stop_simulation(t_sim *sim)
{
	int	i;

	sim->sim_running = 0;
	i = 0;
	while (i < sim->param.num_of_philos)
	{
		sim->philos[i].running_sim = 0;
		i++;
	}
}

static int	check_philosopher_death(t_sim *sim, int i)
{
	long long	current_time;
	long long	time_since_last_meal;

	pthread_mutex_lock(sim->philos[i].time_mutex);
	current_time = current_timestamp_ms();
	time_since_last_meal = current_time - sim->philos[i].last_meal_time;
	if (time_since_last_meal > sim->param.time_to_die
		&& sim->philos[i].state != DEAD)
	{
		sim->philos[i].state = DEAD;
		sim->philos[i].running_sim = 0;
		//printf("before the mutex unlock\n");
		pthread_mutex_unlock(sim->philos[i].time_mutex);
		print_state(&sim->philos[i], "died");
		sim->philos[i].running_sim = 0;
		//	printf("after the mutex unlock\n");
		stop_simulation(sim);
		return (1);
	}
	pthread_mutex_unlock(sim->philos[i].time_mutex);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;
	while (sim->sim_running)
	{
		if (check_meals_completion(sim))
		{
			stop_simulation(sim);
			return (NULL);
		}
		i = 0;
		while (i < sim->param.num_of_philos)
		{
			if (sim->philos[i].state == DEAD || !sim->philos[i].running_sim)
			{
				i++;
				continue ;
			}
			if (check_philosopher_death(sim, i))
				return (NULL);
			i++;
		}
		usleep(500);
	}
	return (NULL);
}
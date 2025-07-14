#include "philosophers.h"

static void	cleanup_philos(t_sim *sim)
{
	int	i;

	if (sim->philos)
	{
		i = 0;
		while (i < sim->param.num_of_philos)
		{
			if (sim->philos[i].time_mutex)
			{
				pthread_mutex_destroy(sim->philos[i].time_mutex);
				free(sim->philos[i].time_mutex);
				sim->philos[i].time_mutex = NULL;
			}
			i++;
		}
		free(sim->philos);
		sim->philos = NULL;
	}
}

static void	cleanup_forks(t_sim *sim)
{
	int	i;

	if (sim->forks)
	{
		i = 0;
		while (i < sim->param.num_of_philos)
		{
			pthread_mutex_destroy(&sim->forks[i].mutex);
			i++;
		}
		free(sim->forks);
		sim->forks = NULL;
	}
}

void	cleanup_sim(t_sim *sim)
{
	cleanup_forks(sim);
	cleanup_philos(sim);
	if (sim->print_mutex)
	{
		pthread_mutex_destroy(sim->print_mutex);
		free(sim->print_mutex);
		sim->print_mutex = NULL;
	}
}
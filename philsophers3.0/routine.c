#include "philosophers.h"

int	routine(t_sim *sim)
{
	pthread_t	monitor_thread;
	int			i;

	sim->sim_running = 1;
	pthread_create(&monitor_thread, NULL, monitor_routine, sim);
	i = 0;
	while (i < sim->param.num_of_philos)
	{
		pthread_create(&sim->philos[i].thread, NULL, thread_routine, &sim->philos[i]);
		i++;
	}
	i = 0;
	while (i < sim->param.num_of_philos)
	{
		pthread_join(sim->philos[i].thread, NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
	return (0);
}
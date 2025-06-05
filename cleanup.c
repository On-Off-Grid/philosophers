#include "philo.h"

void cleanup_simulation(t_simulation *sim)
{
    int i;

    // Destroy fork mutexes
    if (sim->forks)
    {
        i = 0;
        while (i < sim->params.num_philosophers)
        {
            pthread_mutex_destroy(&sim->forks[i].mutex);
            i++;
        }
        free(sim->forks);
    }

    // Free philosophers array
    if (sim->philosophers)
        free(sim->philosophers);

    // Destroy print mutex
    pthread_mutex_destroy(&sim->print_mutex);
}

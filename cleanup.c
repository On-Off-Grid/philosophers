#include "philosophers.h"

void  cleanup_sim(t_sim *sim)
{
  int i;

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
  if (sim->philos)
  {
    free(sim->philos);
    sim->philos = NULL;
  }
  if (sim->print_mutex)
  {
    pthread_mutex_destroy(sim->print_mutex);
    free(sim->print_mutex);
  }
  if (sim->time_mutex)
  {
    pthread_mutex_destroy(sim->time_mutex);
    free(sim->time_mutex);
  }
  pthread_mutex_destroy(sim->print_mutex);
}

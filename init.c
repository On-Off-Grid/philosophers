#include "philosophers.h"

static int validate_args(t_params *param)
{
  if (param->num_of_philos < 1 || param->num_of_philos > 200)
    return (ERR_ARGS);
  if (param->time_to_die < 60 ||param->time_to_eat < 60 || 
      param->time_to_sleep < 60 )
    return (ERR_ARGS);
  if (param->must_eat_time < -1)
    return (ERR_ARGS);
  return (SUCCESS);
}

static int parse_args(t_params *param, int ac, char **av)
{
  if (ac != 5 && ac != 6)
    return (ERR_ARGS);
  param->num_of_philos = atoi(av[1]);
  param->time_to_die= atoi(av[2]);
  param->time_to_eat= atoi(av[3]);
  param->time_to_sleep= atoi(av[4]);
  if (ac == 6)
    param->must_eat_time = atoi(av[5]);
  else
    param->must_eat_time = -1;
  return (validate_args);
}

static int init_forks(t_sim *sim)
{
  int i;
  sim->forks = malloc(sizeof(t_fork) * sim->param.num_of_philos);
  if (!sim->forks)
    return (ERR_MALLOC);
  i = 0;
  while (i < sim->param.num_of_philos)
  {
    sim->forks[i].fork_id = i;
    sim->forks[i].is_taken = 0;
    if (pthread_mutex_init(&sim->forks[i].mutex, NULL) != 0)
      return (ERR_MUTEX);
    i++;
  }
  return (SUCCESS);
}

static int init_philos(t_sim *sim)
{
  int i;

  sim->philos = malloc(sizeof(t_philo) * sim->param.num_of_philos);
  if (!sim->philos)
    return (ERR_MALLOC);
  i = 0;
  while (i < sim->param.num_of_philos)
  {
    sim->philos[i].id = i + 1;
    sim->philos[i].meals_eaten = 0;
    sim->philos[i].state = THINKING;
    sim->philos[i].params = &sim->param;
    sim->philos[i].print_mutex = &sim->print_mutex;
    sim->philos[i].left_fork = &sim->forks[i];
    sim->philos[i].right_fork = &sim->forks[(i + 1) % sim->param.num_of_philos];
    i++;
  }
}

int init_sim(t_sim *sim, int ac, char **av)
{
  int status;

  sim->sim_running = 1;
  status = parse_args(&sim->params, ac, av);
  if (status != SUCCESS)
    return (status);
  if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
    return (ERR_MUTEX);
  status = init_forks(sim);
  if (status != SUCCESS)
    return (status);
  status = init_philos(sim);
  if (status != SUCCESS)
    return (status);
  return (SUCCESS);
}

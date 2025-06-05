#include "philo.h"

static int validate_params(t_params *params)
{
    if (params->num_philosophers < 1 || params->num_philosophers > 200)
        return (ERROR_ARGS);
    if (params->time_to_die < 60 || params->time_to_eat < 60 
        || params->time_to_sleep < 60)
        return (ERROR_ARGS);
    if (params->must_eat_count < -1)
        return (ERROR_ARGS);
    return (SUCCESS);
}

int parse_arguments(t_params *params, int argc, char **argv)
{
    if (argc != 5 && argc != 6)
        return (ERROR_ARGS);

    params->num_philosophers = atoi(argv[1]);
    params->time_to_die = atoi(argv[2]);
    params->time_to_eat = atoi(argv[3]);
    params->time_to_sleep = atoi(argv[4]);
    params->must_eat_count = (argc == 6) ? atoi(argv[5]) : -1;

    return (validate_params(params));
}

static int init_forks(t_simulation *sim)
{
    int i;

    sim->forks = malloc(sizeof(t_fork) * sim->params.num_philosophers);
    if (!sim->forks)
        return (ERROR_MALLOC);

    i = 0;
    while (i < sim->params.num_philosophers)
    {
        sim->forks[i].fork_id = i;
        sim->forks[i].is_taken = 0;
        if (pthread_mutex_init(&sim->forks[i].mutex, NULL) != 0)
            return (ERROR_MUTEX);
        i++;
    }
    return (SUCCESS);
}

static int init_philosophers(t_simulation *sim)
{
    int i;

    sim->philosophers = malloc(sizeof(t_philosopher) * sim->params.num_philosophers);
    if (!sim->philosophers)
        return (ERROR_MALLOC);

    i = 0;
    while (i < sim->params.num_philosophers)
    {
        sim->philosophers[i].id = i + 1;
        sim->philosophers[i].state = THINKING;
        sim->philosophers[i].meals_eaten = 0;
        sim->philosophers[i].params = &sim->params;
        sim->philosophers[i].print_mutex = &sim->print_mutex;
        sim->philosophers[i].left_fork = &sim->forks[i];
        sim->philosophers[i].right_fork = &sim->forks[(i + 1) % sim->params.num_philosophers];
        i++;
    }
    return (SUCCESS);
}

int init_simulation(t_simulation *sim, int argc, char **argv)
{
    int status;

    // Initialize simulation running flag
    sim->simulation_running = 1;

    // Parse and validate arguments
    status = parse_arguments(&sim->params, argc, argv);
    if (status != SUCCESS)
        return (status);

    // Initialize print mutex
    if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
        return (ERROR_MUTEX);

    // Initialize forks (mutexes)
    status = init_forks(sim);
    if (status != SUCCESS)
        return (status);

    // Initialize philosophers
    status = init_philosophers(sim);
    if (status != SUCCESS)
        return (status);

    return (SUCCESS);
}

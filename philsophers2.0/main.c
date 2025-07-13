/* Fixed main.c - Key improvements for timing and starvation issues */

#include "philosophers.h"

long long current_timestamp_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

void interruptible_sleep(int ms, t_philo *philo)
{
    long long start = current_timestamp_ms();
    while (philo->running_sim && current_timestamp_ms() - start < ms)
        usleep(50); // More responsive
}

void print_state(t_philo *philo, char *state)
{
    pthread_mutex_lock(philo->print_mutex);
    if (philo->running_sim)
        printf("%lld %d %s\n", current_timestamp_ms() - philo->params->start_time, philo->id, state);
    pthread_mutex_unlock(philo->print_mutex);
}

void *thread_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;

    // Special case: only one philosopher
    if (philo->params->num_of_philos == 1)
    {
        pthread_mutex_lock(&philo->left_fork->mutex);
        print_state(philo, "has taken a fork");
        interruptible_sleep(philo->params->time_to_die, philo);
        //usleep(philo->params->time_to_die * 1000);
        print_state(philo, "died");
        pthread_mutex_unlock(&philo->left_fork->mutex);
        return NULL;
    }

    // Add small delay for even-numbered philosophers to reduce contention
    if (philo->id % 2 == 0)
        usleep(100);

    while (philo->running_sim)
    {
        // Take forks with consistent ordering to prevent deadlock
        t_fork *first_fork, *second_fork;
        
        if (philo->id % 2 == 1)
        {
            first_fork = philo->left_fork;
            second_fork = philo->right_fork;
        }
        else
        {
            first_fork = philo->right_fork;
            second_fork = philo->left_fork;
        }

        // Take first fork
        pthread_mutex_lock(&first_fork->mutex);
        if (!philo->running_sim)
        {
            pthread_mutex_unlock(&first_fork->mutex);
            break;
        }
        print_state(philo, "has taken a fork");

        // Take second fork
        pthread_mutex_lock(&second_fork->mutex);
        if (!philo->running_sim)
        {
            pthread_mutex_unlock(&second_fork->mutex);
            pthread_mutex_unlock(&first_fork->mutex);
            break;
        }
        print_state(philo, "has taken a fork");

        // Eat
        pthread_mutex_lock(philo->time_mutex);
        philo->last_meal_time = current_timestamp_ms();
        pthread_mutex_unlock(philo->time_mutex);
        philo->state = EATING;
        print_state(philo, "is eating");
        interruptible_sleep(philo->params->time_to_eat, philo);
        philo->meals_eaten++;

        // Release forks immediately if simulation ends
        pthread_mutex_unlock(&second_fork->mutex);
        pthread_mutex_unlock(&first_fork->mutex);
        if (!philo->running_sim) break;

        // Sleep
        philo->state = SLEEPING;
        print_state(philo, "is sleeping");
        interruptible_sleep(philo->params->time_to_sleep, philo);
        if (!philo->running_sim) break;

        // Think
        philo->state = THINKING;
        print_state(philo, "is thinking");
        usleep(100);
    }
    
    return NULL;
}

void *monitor_routine(void *arg)
{
    t_sim *sim = (t_sim *)arg;
    
    while (sim->sim_running)
    {
        // Check if all philosophers have eaten enough meals
        if (sim->param.must_eat_time > 0)
        {
            int all_done = 1;
            for (int i = 0; i < sim->param.num_of_philos; i++)
            {
                if (sim->philos[i].meals_eaten < sim->param.must_eat_time)
                {
                    all_done = 0;
                    break;
                }
            }
            if (all_done)
            {
                sim->sim_running = 0;
                for (int i = 0; i < sim->param.num_of_philos; i++)
                    sim->philos[i].running_sim = 0;
                return NULL;
            }
        }

        // Check for death
        for (int i = 0; i < sim->param.num_of_philos; i++)
        {
            if (sim->philos[i].state == DEAD || !sim->philos[i].running_sim)
                continue;

            pthread_mutex_lock(sim->philos[i].time_mutex);
            long long current_time = current_timestamp_ms();
            long long time_since_last_meal = current_time - sim->philos[i].last_meal_time;
            // Debug log for starvation diagnosis
            // printf("Philo %d: last_meal_time=%lld, time_since_last_meal=%lld\n", i+1, sim->philos[i].last_meal_time, time_since_last_meal);
            // Check if philosopher died
            if (time_since_last_meal > sim->param.time_to_die && sim->philos[i].state != DEAD)
            {
                sim->philos[i].state = DEAD;
                sim->philos[i].running_sim = 0;
                pthread_mutex_unlock(sim->philos[i].time_mutex);
                print_state(&sim->philos[i], "died");
                // Stop simulation
                sim->sim_running = 0;
                for (int j = 0; j < sim->param.num_of_philos; j++)
                    sim->philos[j].running_sim = 0;
                return NULL;
            }
            pthread_mutex_unlock(sim->philos[i].time_mutex);
        }
        
        usleep(500); // Check every 0.5ms for better precision
    }
    
    return NULL;
}

/* Fixed cleanup.c */
void cleanup_sim(t_sim *sim)
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
    
    if (sim->print_mutex)
    {
        pthread_mutex_destroy(sim->print_mutex);
        free(sim->print_mutex);
        sim->print_mutex = NULL;
    }
    
    // if (sim->time_mutex)
    // {
    //     pthread_mutex_destroy(sim->time_mutex);
    //     free(sim->time_mutex);
    //     sim->time_mutex = NULL;
    // }
}

/* Improved argument validation in init.c */
static int validate_args(t_params *param)
{
    if (param->num_of_philos < 1 || param->num_of_philos > 200)
        return (ERR_ARGS);
    if (param->time_to_die < 60 || param->time_to_eat < 60 || 
        param->time_to_sleep < 60)
        return (ERR_ARGS);
    if (param->must_eat_time < -1)
        return (ERR_ARGS);
    return (SUCCESS);
}

int parse_args(t_params *param, int ac, char **av)
{
    if (ac != 5 && ac != 6)
        return (ERR_ARGS);
    
    // Check for negative values
    for (int i = 1; i < ac; i++)
    {
        if (atoi(av[i]) < 0)
            return (ERR_ARGS);
    }
    
    param->num_of_philos = atoi(av[1]);
    param->time_to_die = atoi(av[2]);
    param->time_to_eat = atoi(av[3]);
    param->time_to_sleep = atoi(av[4]);
    param->start_time = current_timestamp_ms();
    
    if (ac == 6)
        param->must_eat_time = atoi(av[5]);
    else
        param->must_eat_time = -1;
    
    return (validate_args(param));
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
    // Removed is_taken field
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
    sim->philos[i].running_sim = 1;
    sim->philos[i].id = i + 1;
    sim->philos[i].meals_eaten = 0;
    sim->philos[i].state = THINKING;
    sim->philos[i].params = &sim->param;
    sim->philos[i].print_mutex = sim->print_mutex;
    // Allocate and initialize a mutex for each philosopher's last_meal_time
    sim->philos[i].time_mutex = malloc(sizeof(pthread_mutex_t));
    if (!sim->philos[i].time_mutex)
      return (ERR_MALLOC);
    if (pthread_mutex_init(sim->philos[i].time_mutex, NULL) != 0)
      return (ERR_MUTEX);
    sim->philos[i].left_fork = &sim->forks[i];
    sim->philos[i].right_fork = &sim->forks[(i + 1) % sim->param.num_of_philos];
    sim->philos[i].last_meal_time = sim->param.start_time;
    i++;
  }
  return (SUCCESS);
}

int init_sim(t_sim *sim, int ac, char **av)
{
  int status;

  sim->sim_running = 1;
  status = parse_args(&sim->param, ac, av);
  if (status != SUCCESS)
    return (status);
  sim->print_mutex = malloc(sizeof(pthread_mutex_t));
  if (!sim->print_mutex)
    return (ERR_MALLOC);
  if (pthread_mutex_init(sim->print_mutex, NULL) != 0)
    return (ERR_MUTEX);
  // Removed global time_mutex, now per-philosopher
  status = init_forks(sim);
  if (status != SUCCESS)
    return (status);
  status = init_philos(sim);
  if (status != SUCCESS)
    return (status);
  return (SUCCESS);
}
int routine(t_sim *sim)
{
    int i;
    pthread_t monitor_thread;

    sim->sim_running = 1;
    pthread_create(&monitor_thread, NULL, monitor_routine, sim);
    i = 0;
    while(i < sim->param.num_of_philos)
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
    return(0);
}

int main(int ac, char **av)
{
    t_sim sim;
    int   status;

    status = init_sim(&sim, ac, av);
    if (status != SUCCESS)
        return (status);
    status = routine(&sim);
    if (status != SUCCESS)
      return (status);
    cleanup_sim(&sim);
    return (0);
}
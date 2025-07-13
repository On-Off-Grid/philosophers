#include "philosophers.h"

long long   current_timestamp_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
    return SUCCESS;
}

void print_state(t_philo *philo, char *state)
{
    if (!philo->running_sim)
        return;
    pthread_mutex_lock(philo->print_mutex);
    if (philo->running_sim)
      printf("%lld %d %s\n", current_timestamp_ms() - philo->params->start_time, philo->id, state); // change later
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
        usleep(philo->params->time_to_die * 1000);
        pthread_mutex_unlock(&philo->left_fork->mutex);
        return NULL;
    }

    while (philo->running_sim && philo->state != DEAD)
    {
        // Check if simulation ended before trying to eat
        if (!philo->running_sim) break;
        
        // Take forks (with deadlock prevention)
        if (philo->id % 2 == 0)
        {
            pthread_mutex_lock(&philo->left_fork->mutex);
            if (!philo->running_sim) {
                pthread_mutex_unlock(&philo->left_fork->mutex);
                break;
            }
            print_state(philo, "has taken a fork");
            
            pthread_mutex_lock(&philo->right_fork->mutex);
            if (!philo->running_sim) {
                pthread_mutex_unlock(&philo->right_fork->mutex);
                pthread_mutex_unlock(&philo->left_fork->mutex);
                break;
            }
            print_state(philo, "has taken a fork");
        }
        else
        {
            pthread_mutex_lock(&philo->right_fork->mutex);
            if (!philo->running_sim) {
                pthread_mutex_unlock(&philo->right_fork->mutex);
                break;
            }
            print_state(philo, "has taken a fork");
            
            pthread_mutex_lock(&philo->left_fork->mutex);
            if (!philo->running_sim) {
                pthread_mutex_unlock(&philo->left_fork->mutex);
                pthread_mutex_unlock(&philo->right_fork->mutex);
                break;
            }
            print_state(philo, "has taken a fork");
        }

        // Eat
        philo->state = EATING;
        
        // Update last meal time with proper synchronization
        pthread_mutex_lock(philo->time_mutex);
        philo->last_meal_time = current_timestamp_ms();
        pthread_mutex_unlock(philo->time_mutex);
        
        print_state(philo, "is eating");
        usleep(philo->params->time_to_eat * 1000);
        philo->meals_eaten++;
        
        // Release forks
        pthread_mutex_unlock(&philo->left_fork->mutex);
        pthread_mutex_unlock(&philo->right_fork->mutex);

        // Check if simulation ended before sleeping
        if (!philo->running_sim) break;
        
        // Sleep
        philo->state = SLEEPING;
        print_state(philo, "is sleeping");
        usleep(philo->params->time_to_sleep * 1000);

        // Check if simulation ended before thinking
        if (!philo->running_sim) break;
        
        // Think
        philo->state = THINKING;
        print_state(philo, "is thinking");
    }
    
    return NULL;
}

/*
void    *thread_routine(void *arg)
{
    t_philo   *philo = (t_philo *)arg;

    // Special case: only one philosopher
    if (philo->params->num_of_philos == 1)
    {
        pthread_mutex_lock(&philo->left_fork->mutex);
        print_state(philo, "has taken a fork");
        usleep(philo->params->time_to_die * 1000);
        pthread_mutex_unlock(&philo->left_fork->mutex);
        return NULL;
    }

    while (philo->state != DEAD && philo->running_sim)
    {
      if (philo->id % 2 == 0)
      {
        pthread_mutex_lock(&philo->left_fork->mutex);
        print_state(philo, "has taken a fork");
        pthread_mutex_lock(&philo->right_fork->mutex);
        print_state(philo, "has taken a fork");
      }
      else
      {
        pthread_mutex_lock(&philo->right_fork->mutex);
        print_state(philo, "has taken a fork");
        pthread_mutex_lock(&philo->left_fork->mutex);
        print_state(philo, "has taken a fork");
      }
      philo->state = EATING;
      pthread_mutex_lock(philo->time_mutex);
      philo->last_meal_time = current_timestamp_ms();
      pthread_mutex_unlock(philo->time_mutex);
      print_state(philo, "is eating");
      usleep(philo->params->time_to_eat * 1000);
      philo->meals_eaten += 1;
      pthread_mutex_unlock(&philo->left_fork->mutex);
      pthread_mutex_unlock(&philo->right_fork->mutex);
      philo->state = SLEEPING;
      print_state(philo, "is sleeping");
      usleep(philo->params->time_to_sleep * 1000);
      philo->state = THINKING;
      print_state(philo, "is thinking");
      //if (philo->meals_eaten <= min(total_meals_eaten) + 1)
        // allow to eat
      //else
        // wait or yield
      // After each major step, check if simulation ended
      if (!philo->running_sim)
        break;
    }
    return NULL;
}
*/
/*
void *monitor_routine(void *arg)
{
    t_sim *sim = (t_sim *)arg;
    int i;
    int j;
    int all_done = 0;

    while (sim->sim_running)
    {
        i = 0;
        while (i < sim->param.num_of_philos)
        {
            pthread_mutex_lock(sim->philos[i].time_mutex);
            long long current_time = current_timestamp_ms();
            long long last_meal = sim->philos[i].last_meal_time;
            pthread_mutex_unlock(sim->philos[i].time_mutex);

            if (current_time - last_meal > sim->param.time_to_die)
            {
                sim->philos[i].state = DEAD;
                print_state(&sim->philos[i], "died");
                sim->sim_running = 0;
                j = 0;
                while (j < sim->param.num_of_philos)
                    sim->philos[j++].running_sim = 0;                   
                break;
            }
            i++;
        }
        usleep(100); // Prevent tight loop, adjust as necessary
    }
    // Fix meal count check in monitor_routine
    if (sim->param.must_eat_time > 0)
    {
        all_done = 1;
        for (i = 0; i < sim->param.num_of_philos; i++)
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
            return (NULL);
        }
    }
    return NULL;
}
*/

void *monitor_routine(void *arg)
{
    t_sim *sim = (t_sim *)arg;
    int i;
    int all_done = 0;

    while (sim->sim_running)
    {
        // Check if all philosophers have eaten enough meals (if required)
        if (sim->param.must_eat_time > 0)
        {
            all_done = 1;
            for (i = 0; i < sim->param.num_of_philos; i++)
            {
                if (sim->philos[i].meals_eaten < sim->param.must_eat_time)
                {
                    all_done = 0;
                    break;
                }
            }
            if (all_done)
            {
                // Stop simulation - everyone has eaten enough
                sim->sim_running = 0;
                for (i = 0; i < sim->param.num_of_philos; i++)
                    sim->philos[i].running_sim = 0;
                return NULL;
            }
        }

        // Check for death
        i = 0;
        while (i < sim->param.num_of_philos)
        {
            // Don't check already dead philosophers
            if (sim->philos[i].state == DEAD)
            {
                i++;
                continue;
            }

            pthread_mutex_lock(sim->philos[i].time_mutex);
            long long current_time = current_timestamp_ms();
            long long last_meal = sim->philos[i].last_meal_time;
            pthread_mutex_unlock(sim->philos[i].time_mutex);

            // Check if philosopher should die
            if (current_time - last_meal > sim->param.time_to_die)
            {
                // Mark philosopher as dead
                sim->philos[i].state = DEAD;
                print_state(&sim->philos[i], "died");
                
                // Stop the entire simulation
                sim->sim_running = 0;
                for (int j = 0; j < sim->param.num_of_philos; j++)
                    sim->philos[j].running_sim = 0;
                
                return NULL; // Exit monitor thread immediately
            }
            i++;
        }
        
        usleep(1000); // Check every 1ms for better precision
    }
    
    return NULL;
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



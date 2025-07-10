#include "philosophers.h"

long long   current_timestamp_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

void print_state(t_philo *philo, char *state, long long time)
{
        pthread_mutex_lock(philo->print_mutex);
        printf("Timestamp %d %s\n", philo->id, state); // change later
        pthread_mutex_unlock(philo->print_mutex);
}

void    *thread_routine(void *arg)
{
    t_philo   *philo;

    philo = (t_philo*)arg;
    while (philo->state != DEAD)
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
      philo->last_meal_time = 11;
      print_state(philo, "is eating");
      usleep(philo->params->time_to_eat);
      philo->meals_eaten += 1;
      pthread_mutex_unlock(&philo->left_fork->mutex);
      pthread_mutex_unlock(&philo->right_fork->mutex);
      philo->state = SLEEPING;
      sleep(philo->params->time_to_sleep);
      philo->state = THINKING;
      print_state(philo, "is thinking");
      /*if (philo->meals_eaten <= min(total_meals_eaten) + 1)
        // allow to eat
      else
        // wait or yield
    }*/
    if (philo->state == DEAD)
      print_state(philo, "died");
}

int routine(t_sim *sim)
{
    int i;

    i = 0;
    while(sim->sim_running)
    {
        while(i < sim->param.num_of_philos)
        {
            pthread_create(&sim->philos[i].thread, NULL, thread_routine, &sim->philos[i]);
            i++;
        }
    }
}

int main(int ac, char ** av)
{
    t_sim sim;
    int   status;

    status = init_sim(&sim, ac, av);
    if (status != SUCCESS)
        return (status);

    // TODO implenet philosophers routine
    status = routine(&sim);

    cleanup_sim(&sim);
}

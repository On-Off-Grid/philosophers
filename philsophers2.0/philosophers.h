#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H 

# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

// remove
# include <stdio.h>

// ERROR codes
# define SUCCESS 0
# define ERR_ARGS 1
# define ERR_MALLOC 2
# define ERR_MUTEX 3
# define ERR_THREAD 4

// Philosopher state
# define THINKING 0
# define EATING 1
# define SLEEPING 2
# define DEAD 3
# define FORK 4

// Shared parameters struct
typedef  struct s_params
{
  int       num_of_philos;
  int       time_to_die;
  int       time_to_eat;
  int       time_to_sleep;
  int       must_eat_time;
  long long start_time;
}     t_params;

// fork (mutex) struct
typedef struct s_fork
{
  int             fork_id;
  int             is_taken;
  pthread_mutex_t mutex;
}     t_fork;

// Philosopher struct
typedef struct s_philo
{
  int             id;
  int             meals_eaten;
  int             state;
  int             running_sim;
  t_params        *params;
  pthread_mutex_t *print_mutex;
  pthread_mutex_t *time_mutex;
  t_fork          *left_fork;
  t_fork          *right_fork;
  long long       last_meal_time;
  pthread_t       thread;
}       t_philo;

// Simulation struct
typedef  struct s_sim          
{                              
  t_params        param;             
  t_philo         *philos;           
  t_fork          *forks;            
  pthread_mutex_t *print_mutex;
  pthread_mutex_t *time_mutex;
  int             sim_running;       
}     t_sim;    



// Functions prototypes:

int init_sim(t_sim *sim, int ac, char **av);
int parse_args(t_params *params, int ac, char **av);
void  cleanup_sim(t_sim *sim);
void *thread_routine(void *arg);
void *monitor_routine(void *arg);
void print_state(t_philo *philo, char *state);
int routine(t_sim *sim);
/*void destroy_forks(t_fork *forks, int num_of_forks);
void destroy_philos(t_philo *philos, int num_of_philos);
void destroy_sim(t_sim *sim);
void init_forks(t_fork *forks, int num_of_forks);
void init_philos(t_philo *philos, t_fork *forks, int num_of_philos, t_params *params);
void init_simulation(t_sim *sim, int ac, char **av);
void print_error(int error_code);*/
long long   current_timestamp_ms(void);

#endif

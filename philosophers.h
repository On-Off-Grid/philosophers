#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H 

# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

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

// Simulation struct
typedef  struct s_sim          
{                              
  t_params        param;             
  t_philo         *philos;           
  t_fork          *forks;            
  pthread_mutex_t *print_mutex;
  int             sim_running;       
}     t_simu;    

// Philosopher struct
typedef struct s_philo
{
  int             id;
  int             meals_eaten;
  int             state;
  t_params        *params;
  pthread_mutex_t *print_mutex;
  t_fork          *left_fork;
  t_fork          *right_fork;
  long long       last_meal_time;
  pthread_t       thread;
}       t_philo;

// Functions prototypes:

int init_sim(t_sim *sim, int ac, char **av);
int parse_args(t_params *params, int ac, char **av);

#endif

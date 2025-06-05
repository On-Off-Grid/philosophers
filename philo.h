#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/time.h>
# include <unistd.h>

/* Error Codes */
# define SUCCESS 0
# define ERROR_ARGS 1
# define ERROR_MALLOC 2
# define ERROR_MUTEX 3
# define ERROR_THREAD 4

/* Philosopher States */
# define THINKING 0
# define EATING 1
# define SLEEPING 2
# define DEAD 3

/* Simulation Parameters Structure */
typedef struct s_params
{
    int             num_philosophers;
    int             time_to_die;      // in milliseconds
    int             time_to_eat;      // in milliseconds
    int             time_to_sleep;    // in milliseconds
    int             must_eat_count;   // optional parameter (-1 if not specified)
    long long       start_time;       // timestamp when simulation starts
}               t_params;

/* Fork (Mutex) Structure */
typedef struct s_fork
{
    pthread_mutex_t mutex;
    int            fork_id;
    int            is_taken;
}               t_fork;

/* Philosopher Structure */
typedef struct s_philosopher
{
    int             id;
    pthread_t       thread;
    int             state;
    int            meals_eaten;
    long long      last_meal_time;
    t_fork         *left_fork;
    t_fork         *right_fork;
    t_params       *params;          // pointer to shared parameters
    pthread_mutex_t *print_mutex;    // pointer to shared printing mutex
}               t_philosopher;

/* Simulation Structure - holds all data */
typedef struct s_simulation
{
    t_params        params;
    t_philosopher   *philosophers;
    t_fork         *forks;
    pthread_mutex_t print_mutex;
    int            simulation_running;
}               t_simulation;

/* Function Prototypes */
int     init_simulation(t_simulation *sim, int argc, char **argv);
void    cleanup_simulation(t_simulation *sim);
int     parse_arguments(t_params *params, int argc, char **argv);
void    *philosopher_routine(void *arg);

#endif

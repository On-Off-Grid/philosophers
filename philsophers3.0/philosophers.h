#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdio.h>

# define SUCCESS 0
# define ERR_ARGS 1
# define ERR_MALLOC 2
# define ERR_MUTEX 3
# define ERR_THREAD 4

# define THINKING 0
# define EATING 1
# define SLEEPING 2
# define DEAD 3
# define FORK 4

typedef struct s_params
{
	int			num_of_philos;
	int			time_to_die;
	int			time_to_eat;
	int			time_to_sleep;
	int			must_eat_time;
	long long	start_time;
}	t_params;

typedef struct s_fork
{
	int				fork_id;
	pthread_mutex_t	mutex;
}	t_fork;

typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	int				state;
	int				running_sim;
	t_params		*params;
	pthread_mutex_t	*print_mutex;
	pthread_mutex_t	*time_mutex;
	t_fork			*left_fork;
	t_fork			*right_fork;
	long long		last_meal_time;
	pthread_t		thread;
}	t_philo;

typedef struct s_sim
{
	t_params		param;
	t_philo			*philos;
	t_fork			*forks;
	pthread_mutex_t	*print_mutex;
	int				sim_running;
}	t_sim;

/* Function prototypes */
int			init_sim(t_sim *sim, int ac, char **av);
int			parse_args(t_params *params, int ac, char **av);
void		cleanup_sim(t_sim *sim);
void		*thread_routine(void *arg);
void		*monitor_routine(void *arg);
void		print_state(t_philo *philo, char *state);
int			routine(t_sim *sim);
long long	current_timestamp_ms(void);
void		interruptible_sleep(int ms, t_philo *philo);

#endif
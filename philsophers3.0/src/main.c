#include "philosophers.h"

int	main(int ac, char **av)
{
	t_sim	sim;
	int		status;

	status = init_sim(&sim, ac, av);
	if (status != SUCCESS)
		return (status);
	status = routine(&sim);
	if (status != SUCCESS)
		return (status);
	cleanup_sim(&sim);
	return (0);
}
#include "philo.h"

int main(int argc, char **argv)
{
    t_simulation sim;
    int         status;

    // Initialize simulation with parsed arguments
    status = init_simulation(&sim, argc, argv);
    if (status != SUCCESS)
        return (status);

    // Create threads and start simulation
    // TODO: Implement simulation start logic

    // Cleanup resources
    cleanup_simulation(&sim);
    return (SUCCESS);
}

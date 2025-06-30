#include "philosophers.h"

int main(int ac, char ** av)
{
    t_sim sim;
    int   status;

    status = init_sim(&sim, ac, av);
    if (status != SUCCESS)
        return (status);

    philos_routine();


    cleanup_sim(&sim);
}

#ifndef SCHEDULER_H 
#define SCHEDULER_H 

#include "process.h"
#include "queue.h"
#include <stdint.h>

void run_simulation(Process **all_procs, int num_procs, 
int num_levels, uint32_t *quantums, uint32_t boost_interval);

#endif

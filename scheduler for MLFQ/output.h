#ifndef OUTPUT_H
#define OUTPUT_H

#include "process.h"
#include <stdint.h>

void print_running(uint32_t tick, const char *pid, int level,
uint32_t rtime, uint32_t rquantum);

void print_blocked(uint32_t tick, const char *pid);

void print_unblocked(uint32_t tick, int num_levels, 
Process **sorted, int count);

void print_finished(uint32_t tick, const char *pid);

void print_boost(uint32_t tick);

void print_stats(uint64_t avg_turnaround, uint64_t avg_waiting, uint64_t max_wait);

void print_preempted(uint32_t t, const char *pid, 
int level, uint32_t rquantum, const char *reason );

#endif


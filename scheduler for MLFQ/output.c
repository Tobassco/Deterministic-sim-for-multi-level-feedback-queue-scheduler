#include "output.h"
#include <stdio.h>

void print_running(uint32_t tick, const char *pid, int level,
uint32_t rtime, uint32_t rquantum) {
    printf("%u,RUNNING,process=%s,queue=%d,"
            "remaining-cpu=%u,remaining-quantum=%u\n",
              tick, pid, level, rtime, rquantum);
}

void print_blocked(uint32_t tick, const char *pid) {
    printf("%u,BLOCKED,process=%s\n", tick, pid);
}

// print one unblocked line for every effected queue 
// sorted[] is sorted by PID so process at the same level appear together
void print_unblocked(uint32_t tick, int num_levels, 
Process **sorted, int count) {
    for (int i = 0; i < num_levels; i++) {
        int first = 1;
        for (int j = 0; j < count; j++) {
            if (sorted[j]->queue_level != i) {
                continue;
            } 
            if (first) {
                printf("%u,UNBLOCKED,queue=%d,processes=", tick, i);
                first = 0;
            } else {
                printf(",");
            }
            printf("%s", sorted[j]->pid);
        }
        if (!first) {
            printf("\n");
        }
    }
}

void print_preempted(uint32_t tick, const char *pid, int level, 
uint32_t rquantum, const char *reason) {
    printf("%u,PREEMPTED,process=%s,queue=%d,"
    "remaining-quantum=%u,reason=%s\n", tick, pid, level, rquantum, reason);
}

void print_finished(uint32_t tick, const char *pid) {
    printf("%u,FINISHED,process=%s\n", tick, pid);
}

void print_boost(uint32_t tick) {
    printf("%u,BOOST\n", tick);
}

void print_stats(uint64_t avg_turnaround, uint64_t avg_wait, uint64_t max_wait) {
    printf("Average turnaround time %llu\n",(unsigned long long)avg_turnaround);
    printf("Average waiting time %llu\n",(unsigned long long)avg_wait);
    printf("Maximum waiting time %llu\n",(unsigned long long)max_wait);
}

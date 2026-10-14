#include "process.h"
#include <stdlib.h>
#include <string.h>


Process *process_create(const char *pid, uint32_t arrival, uint32_t *bursts, int burst_count){
    Process *p = malloc(sizeof(Process));
    strncpy(p -> pid, pid, 8);
    p -> pid[8] = '\0';
    p -> arrival = arrival;
    p -> burst_count = burst_count;
    p -> bursts = malloc(burst_count * sizeof(uint32_t));
    memcpy(p -> bursts, bursts, burst_count * sizeof(uint32_t));
    p -> burst_index = 0;
    p -> remaining_cpu = bursts[0];
    p -> remaining_io = 0;
    p -> state = STATE_READY;
    p -> remaining_quantum = 0;
    p -> queue_level = 0;
    p -> quantum_preserved = 0;
    p -> finish_time = 0;
    p -> next = NULL;
    return p;
}

void process_free(Process *p){
    free(p -> bursts);
    free(p);
}

// sum all cpu bursts including indices
uint64_t process_total_cpu(const Process *p){
    uint64_t total = 0;
    for (int i = 0; i < p -> burst_count; i+=2){
        total += p -> bursts[i];
        }
    return total;
}


// sum of all io bursts including indices
uint64_t process_total_io(const Process *p){
    uint64_t total = 0;
    for (int i = 1; i < p -> burst_count; i+=2){
        total += p -> bursts[i];
        }
    return total;
}
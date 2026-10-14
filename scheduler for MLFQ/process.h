#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

/* max PID length and max bursts*/
#define MAX_PID_LEN 9

#define MAX_BURSTS 127 

/* process states */

typedef enum{
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED,
    STATE_FINISHED
} ProcessState;

typedef struct Process{
    char pid[MAX_PID_LEN];
    uint32_t arrival;
    uint32_t *bursts;
    int burst_count;
    int burst_index;
    uint32_t remaining_cpu;
    uint32_t remaining_io;
    ProcessState state;
    uint32_t remaining_quantum;
    int queue_level;
    int quantum_preserved;
    uint32_t finish_time;
    struct Process *next; 
} Process;

Process *process_create(const char *pid, uint32_t arrival, uint32_t *bursts, int burst_count);
void process_free(Process *p);
uint64_t process_total_cpu(const Process *p);
uint64_t process_total_io(const Process *p);

#endif

#include "scheduler.h"
#include "output.h"
#include <stdlib.h>
#include <string.h>

// compare PID using lexicography (for qsort)
static int cmp_pid(const void *a, const void *b){
    return strcmp((*(const Process **)a)-> pid,
                (*(const Process **)b)-> pid);
}

/* unlinks specific process from the 
linked list for blocked processes */
static void blocked_unlink(Process **head, Process *target){
    while(*head){
        if (*head == target){
            *head = target -> next;
            target-> next = NULL;
            return;
        }
        head = &(*head) -> next;
    }
}

// bring process down by one level, respecting ceiling
static void demote(Process *p, int num_levels){
    if (p->queue_level < num_levels - 1){
        p->queue_level++;
    }
}

static int higher_priority_ready(ReadyQueue *queues, int running_level){
    for (int i = 0; i < running_level; i++){
        if(!queue_empty(&queues[i])){
            return 1;
        }
    }
    return 0;
}

void run_simulation(Process **all_procs, int num_procs,
int num_levels, uint32_t *quantums, uint32_t boost_interval){
    //initialise ready queue for each level
    ReadyQueue *queues = malloc(num_levels * sizeof(ReadyQueue));
    for (int i = 0; i < num_levels; i++)
        queue_init(&queues[i], quantums[i]);
    Process *running = NULL;
    Process *blocked_head = NULL;
    int next_arrival = 0;
    int finished = 0;

    // scratch buffer big enough for all processes
    Process **scratch = malloc(num_procs * sizeof(Process *));

    uint32_t tick = 0;
    while(finished < num_procs){
        // step 1a: resolve running process 
        if(running){
            if(running -> remaining_cpu == 0){
                // cpu burst finished
                int q_done = (running -> remaining_quantum == 0);
                running -> burst_index++;
                
                if(running -> burst_index >= running -> burst_count){
                    // last burst = process finished 
                    running -> finish_time = tick;
                    running -> state = STATE_FINISHED;
                    print_finished(tick, running -> pid);
                    finished++;
                    running = NULL;
                } else{
                    // next burst is IO process blocks
                    if(q_done){
                        demote(running, num_levels);
                    }
                    running -> remaining_io = 
                    running -> bursts[running->burst_index];
                    running -> state = STATE_BLOCKED;
                    running -> next = blocked_head;
                    blocked_head = running; 
                    print_blocked(tick, running ->pid);
                    running = NULL;

                }
            } else if (running -> remaining_quantum == 0){
                // quantum exhausted, cpu burst not done, preempt and demote
                int old_level = running -> queue_level;
                demote(running, num_levels);
                print_preempted(tick, running -> pid, old_level, 0, "quantum");
                running -> state = STATE_READY;
                running -> quantum_preserved = 0; 
                queue_enqueue(&queues[running -> queue_level], running);
                running = NULL;
            }
            // else the process continues running in this tick
        }

        // step 1b, complete IOs

        int ub_count = 0;
        for (Process *p = blocked_head; p ;p = p->next){
            if(p->remaining_io == 0){
                scratch[ub_count++] = p;
            }
        }
        if(ub_count > 0){
            //burst state moves ahead for each unblocked process
            for(int i = 0; i < ub_count; i++){
                Process *p = scratch[i];
                blocked_unlink(&blocked_head, p);
                p -> burst_index++;
                p -> remaining_cpu = p -> bursts[p->burst_index];
                p->state = STATE_READY;
            }
            // sort by PID, then print
            qsort(scratch, ub_count, sizeof(Process  *), cmp_pid);
            print_unblocked(tick, num_levels, scratch, ub_count);
            for (int i = 0; i < ub_count; i++){
                queue_enqueue(&queues[scratch[i]->queue_level], scratch[i]);

            }


        }


        // step 2, add arrivals to Q0
        int arr = 0;
        while(next_arrival < num_procs && 
        all_procs[next_arrival]->arrival == tick){
            scratch[arr++] = all_procs[next_arrival++];
        }

        if (arr > 1){
            qsort(scratch, arr, sizeof(Process *), cmp_pid);
        }

        for (int i = 0; i < arr; i++){
            scratch[i] -> state = STATE_READY;
            scratch[i] -> queue_level = 0;
            queue_enqueue(&queues[0], scratch[i]);
        }


        // priority boosting
        if (boost_interval > 0 && tick > 0 && 
        tick % boost_interval == 0 && finished < num_procs){
            print_boost(tick);
            int b = 0;
            // preempt running process first if it exists 

            if (running){
                print_preempted(tick, running -> pid, running -> queue_level, 
                running -> remaining_quantum, "boosting");
                running -> state = STATE_READY;
                scratch[b++] = running;
                running = NULL;
            }
            
            //drain ready queues
            for(int i = 0; i < num_levels; i++){
                while(!queue_empty(&queues[i])){
                    scratch[b++] = queue_dequeue(&queues[i]);

                }
            }

            // reinsert Q0 in ascending PID order 
            qsort(scratch, b, sizeof(Process *), cmp_pid);
            for(int i = 0; i < b; i++){
                scratch[i] -> queue_level = 0;
                scratch[i] -> quantum_preserved = 0;
                queue_enqueue(&queues[0], scratch[i]);
            }

            // reset blocked process priority to Q0  
            for (Process *p = blocked_head; p; p = p->next){
                p -> queue_level = 0;
            }
        }

        // step 4 cpu states
        //priority preemption: higher priority process became ready 
        if (running && 
        higher_priority_ready(queues, running -> queue_level)){
            int old_level = running -> queue_level;
            uint32_t rq = running -> remaining_quantum;
            running -> state = STATE_READY;
            running -> quantum_preserved = 1; 
            queue_enqueue(&queues[running -> queue_level], running);
            print_preempted(tick, running -> pid, old_level, rq, "priority");
            running = NULL;
        }

        // select next process (if cpu isnt running)
        if(!running){
            for(int i = 0; i < num_levels; i++){
                if(!queue_empty(&queues[i])){
                    running = queue_dequeue(&queues[i]);
                    running -> state = STATE_RUNNING;
                    if(!running -> quantum_preserved){
                        running -> remaining_quantum = queues[i].quantum;
                    }
                    running -> quantum_preserved = 0;
                    
                    print_running(tick, running ->pid, running -> queue_level, 
                    running-> remaining_cpu, running-> remaining_quantum);
                    
                    break;
                }

            }
        }
        // step 5: execute tick
        if(running) {
            running -> remaining_cpu--;
            running->remaining_quantum--;
        }
        for(Process *p = blocked_head; p; p = p->next){
            p-> remaining_io--;
        }
        tick++;

    }

    // report 
    uint64_t sum_tt = 0, sum_wt = 0, max_wt = 0;
    for(int i = 0; i < num_procs; i++){
        Process *p = all_procs[i];
        uint64_t tt = p -> finish_time - p-> arrival; 
        uint64_t wt = tt - process_total_cpu(p) - process_total_io(p);
        sum_tt += tt;
        sum_wt += wt;
        if(wt > max_wt){
            max_wt = wt;
        }
    }

    uint64_t n = (uint64_t)num_procs;
    uint64_t avg_tt = (sum_tt + n - 1) / n;
    uint64_t avg_wt = (sum_wt + n - 1) / n;
    print_stats(avg_tt, avg_wt, max_wt);

    free(scratch);
    free(queues);    
}
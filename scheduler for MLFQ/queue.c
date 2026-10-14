#include "queue.h"
#include <stddef.h>


void queue_init(ReadyQueue *q, uint32_t quantum) {
    q->head = NULL;
    q->tail = NULL;
    q->quantum = quantum;
}

//append to tail
void queue_enqueue(ReadyQueue *q, Process *p) {
    p->next = NULL;
    if (q->tail) {
        q->tail ->next = p;
    } else {
        q->head = p;
    }
    q->tail = p;
}

// remove from head
Process *queue_dequeue(ReadyQueue *q) {
    if (!q->head) {
        return NULL;
    }
    Process *p = q->head;
    q->head = p->next;
    if (!q->head) {
        q->tail = NULL;
    }
    p->next = NULL; 
    return p;
}


int queue_empty(const ReadyQueue *q) {
    return q->head == NULL;
}
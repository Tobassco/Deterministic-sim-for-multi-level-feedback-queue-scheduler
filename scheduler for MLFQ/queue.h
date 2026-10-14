#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct {
    Process *head;
    Process *tail;
    uint32_t quantum; //acts as size of queue
} ReadyQueue;

void queue_init(ReadyQueue *q, uint32_t quantum);
void queue_enqueue(ReadyQueue *q, Process *p);
Process *queue_dequeue(ReadyQueue *q);
int queue_empty(const ReadyQueue *q);

#endif

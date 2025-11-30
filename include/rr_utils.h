#ifndef RR_UTILS_H
#define RR_UTILS_H

#include "process.h"

typedef struct {
    Process** queue;
    int front;
    int rear;
    int size;
    int capacity;
} ReadyQueue;

void init_ready_queue(ReadyQueue* q, int capacity);
void enqueue(ReadyQueue* q, Process* p);
Process* dequeue(ReadyQueue* q);
int queue_empty(ReadyQueue* q);
void free_ready_queue(ReadyQueue* q);

#endif

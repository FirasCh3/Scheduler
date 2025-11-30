#include "rr_utils.h"
#include <stdlib.h>

void init_ready_queue(ReadyQueue* q, int capacity) {
    q->queue = malloc(sizeof(Process*) * capacity);
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    q->capacity = capacity;
}

void enqueue(ReadyQueue* q, Process* p) {
    if (p == NULL) return;
    if (p->in_queue) return; 
    if (q->size == q->capacity) return; 
    q->queue[q->rear] = p;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;
    p->in_queue = 1;
}

Process* dequeue(ReadyQueue* q) {
    if (q->size == 0) return NULL;
    Process* p = q->queue[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    p->in_queue = 0;
    return p;
}

int queue_empty(ReadyQueue* q) {
    return q->size == 0;
}

void free_ready_queue(ReadyQueue* q) {
    free(q->queue);
}

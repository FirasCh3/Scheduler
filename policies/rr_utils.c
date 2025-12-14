#include "rr_utils.h"
#include <stdlib.h>
#include <stdio.h>

void init_ready_queue(ReadyQueue *q, int capacity)
{
    if (q == NULL)
    {
        fprintf(stderr, "Error: NULL queue pointer\n");
        return;
    }

    if (capacity <= 0)
    {
        fprintf(stderr, "Error: Invalid capacity\n");
        return;
    }
    q->queue = malloc(sizeof(Process *) * capacity);
    if (q->queue == NULL)
    {
        fprintf(stderr, "Error: Failed to allocate ready queue\n");
        exit(EXIT_FAILURE);
    }

    q->front = 0;
    q->rear = 0;
    q->size = 0;
    q->capacity = capacity;
};

void enqueue(ReadyQueue *q, Process *p)
{
    if (q == NULL)
    {
        fprintf(stderr, "Error: NULL queue pointer in enqueue\n");
        return;
    }

    if (p == NULL)
        return;

    if (q->queue == NULL)
    {
        fprintf(stderr, "Error: Queue not initialized\n");
        return;
    }

    if (p->in_queue)
        return;

    if (q->size >= q->capacity)
    {
        fprintf(stderr, "Warning: Queue full, cannot enqueue %s\n", p->name);
        return;
    }
    q->queue[q->rear] = p;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;
    p->in_queue = 1;
}

Process *dequeue(ReadyQueue *q)
{
    if (q == NULL || q->queue == NULL)
    {
        fprintf(stderr, "Error: NULL queue in dequeue\n");
        return NULL;
    }
    if (q->size == 0)
        return NULL;
    Process *p = q->queue[q->front];
    if (p == NULL)
    {
        fprintf(stderr, "Error: NULL process in queue\n");
        return NULL;
    }
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    p->in_queue = 0;
    return p;
}

int queue_empty(ReadyQueue *q)
{   if (q == NULL) return 1;
    return q->size == 0;
}

void free_ready_queue(ReadyQueue* q) {
    if (q == NULL) return;
    
    if (q->queue != NULL) {
        free(q->queue);
        q->queue = NULL;
    }
    
    q->size = 0;
    q->capacity = 0;
}
#include <string.h>
#include "process.h"

void init_process(Process* p, const char* name, int arrival, int burst, int priority) {
    strncpy(p->name, name, 32);
    p->arrival_time = arrival;
    p->burst_time = burst;
    p->remaining_time = burst;
    p->priority = priority;
    p->state = NEW;
    p->start_time = -1;
    p->finish_time = -1;
}
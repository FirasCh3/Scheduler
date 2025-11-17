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

int all_finished(ProcessList* plist) {
    for (int i = 0; i < plist->count; i++) {
        if (plist->list[i].state != FINISHED)
            return 0;
    }
    return 1;
}

void update_ready_processes(ProcessList* plist, int current_time) {
    for (int i = 0; i < plist->count; i++) {
        Process* p = &plist->list[i];
        if (p->state == NEW && p->arrival_time == current_time) {
            p->state = READY;
        }
    }
}

void run_process_for_one_unit(Process* p, int current_time) {
    if (p->state != RUNNING)
        p->start_time = (p->start_time == -1) ? current_time : p->start_time;

    p->state = RUNNING;
    p->remaining_time--;

    if (p->remaining_time == 0) {
        p->state = FINISHED;
        p->finish_time = current_time + 1;
    } else {
        p->state = READY;
    }
}
#include "rr.h"
#include "rr_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ProcessList schedule_rr(ProcessList* plist, int time_quantum) {
    int current_time = 0;
    int current_slice = 0;
    Process* current = NULL;

    ProcessList execution_stack;
    execution_stack.count = 0;
    execution_stack.list = NULL;

    ReadyQueue ready_queue;
    init_ready_queue(&ready_queue, plist->count);

    while (!all_finished(plist)) {
         update_ready_processes(plist, current_time);

         for (int i = 0; i < plist->count; i++) {
            Process* p = &plist->list[i];
            if (p->state == READY  && !p->in_queue && p != current) {
                enqueue(&ready_queue, p); 
            }
        }

         if (current == NULL || current_slice == time_quantum || current->state == FINISHED) {
            if (current != NULL && current->state != FINISHED) {
                current->state = READY;
                enqueue(&ready_queue, current);  
            }

            current = dequeue(&ready_queue);
            current_slice = 0;

            if (current != NULL && current->start_time == -1)
                current->start_time = current_time;
        }

         execution_stack.list = realloc(
            execution_stack.list,
            (execution_stack.count + 1) * sizeof(Process)
        );

         
        if (current == NULL) {
            printf("Time %d: CPU Idle\n", current_time);

            Process idle;
            strncpy(idle.name, "idle", 32);
            idle.arrival_time = 0;
            idle.burst_time = 0;
            idle.remaining_time = 0;
            idle.priority = 0;
            idle.state = FINISHED;
            idle.start_time = -1;
            idle.finish_time = -1;

            execution_stack.list[execution_stack.count] = idle;
            execution_stack.count++;
            current_time++;
            continue;
        }

        run_process_for_one_unit(current, current_time);
        current_slice++;
        printf("Time %d: Running %s\n", current_time, current->name);
        execution_stack.list[execution_stack.count] = *current;
        execution_stack.count++;

        current_time++;
    }

    free_ready_queue(&ready_queue);
    return execution_stack;
}

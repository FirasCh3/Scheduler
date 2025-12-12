#include "rr.h"
#include "rr_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ProcessList schedule_rr(ProcessList *plist, int time_quantum)
{
    ProcessList empty_list = {NULL, 0};
    if (plist == NULL)
    {
        fprintf(stderr, "Error: NULL process list\n");
        return empty_list;
    }

    if (plist->list == NULL || plist->count <= 0)
    {
        fprintf(stderr, "Error: Invalid process list\n");
        return empty_list;
    }

    if (time_quantum <= 0)
    {
        fprintf(stderr, "Error: Invalid time quantum (must be > 0)\n");
        return empty_list;
    }

    int current_time = 0;
    int current_slice = 0;
    Process *current = NULL;

    ProcessList execution_stack;
    execution_stack.count = 0;
    execution_stack.list = NULL;

    ReadyQueue ready_queue;
    init_ready_queue(&ready_queue, plist->count);
    if (ready_queue.queue == NULL)
    {
        fprintf(stderr, "Error: Failed to initialize ready queue\n");
        return empty_list;
    }

    while (!all_finished(plist))
    {
        update_ready_processes(plist, current_time);

        for (int i = 0; i < plist->count; i++)
        {
            Process *p = &plist->list[i];
            if (p->state == READY && !p->in_queue && p != current)
            {
                enqueue(&ready_queue, p);
            }
        }

        if (current == NULL || current_slice == time_quantum || current->state == FINISHED)
        {
            if (current != NULL && current->state != FINISHED)
            {
                current->state = READY;
                enqueue(&ready_queue, current);
            }

            current = dequeue(&ready_queue);
            current_slice = 0;

            if (current != NULL && current->start_time == -1)
                current->start_time = current_time;
        }

        Process *new_list = realloc(
        execution_stack.list,
        (execution_stack.count + 1) * sizeof(Process));

        if (new_list == NULL)
        {
            fprintf(stderr, "Error: Memory allocation failed at time %d\n",
                    current_time);
            free(execution_stack.list);
            free_ready_queue(&ready_queue);
            return empty_list;
        }
        execution_stack.list = new_list;

        if (current == NULL)
        {
            printf("Time %d: CPU Idle\n", current_time);

            Process idle;
            strncpy(idle.name, "idle", 31);
            idle.name[31] = '\0';
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
        printf("Time %d: Running %s\n (remaining: %d)\n", current_time, current->name,current->remaining_time);
        execution_stack.list[execution_stack.count] = *current;
        execution_stack.count++;
        current_time++;
    }

    free_ready_queue(&ready_queue);
    return execution_stack;
}

#include "preemptive-priority.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>

ProcessList schedule_preemptive_priority(ProcessList *plist) {
  int current_time = 0;
  Process *current = NULL;

  ProcessList execution_stack;
  execution_stack.count = 0;
  execution_stack.list = NULL;

  while (!all_finished(plist)) {

    update_ready_processes(plist, current_time);

    current = NULL;
    for (int i = 0; i < plist->count; i++) {
      Process *p = &plist->list[i];

      if (p->state == READY && p->remaining_time > 0) {
        if (current == NULL || p->priority > current->priority) {
          current = p;
        }
      }
    }

    if (current != NULL && current->start_time == -1) {
      current->start_time = current_time;
    }

    execution_stack.list = realloc(
        execution_stack.list, (execution_stack.count + 1) * sizeof(Process));

    if (current == NULL) {
      printf("Time %d: CPU Idle\n", current_time);

      Process idle;
      init_process(&idle, "idle", 0, 0, 0);
      idle.state = FINISHED;

      execution_stack.list[execution_stack.count] = idle;
      execution_stack.count++;

      current_time++;
      continue;
    }

    current->state = RUNNING;
    printf("Time %d: Running %s\n", current_time, current->name);

    current->remaining_time--;

    if (current->remaining_time == 0) {
      current->state = FINISHED;
      current->finish_time = current_time + 1;
    } else {
      current->state = READY;
    }

    execution_stack.list[execution_stack.count] = *current;
    execution_stack.count++;

    current_time++;
  }

  return execution_stack;
}

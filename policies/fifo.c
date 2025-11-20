#include "fifo.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
ProcessList schedule_fifo(ProcessList *plist) {
  int current_time = 0;
  Process *current = NULL;
  ProcessList execution_stack;
  execution_stack.list = malloc(sizeof(Process));
  execution_stack.count = 1;
  while (!all_finished(plist)) {
    update_ready_processes(plist, current_time);

    if (current == NULL || current->state == FINISHED) {
      current = NULL;

      for (int i = 0; i < plist->count; i++) {
        Process *p = &plist->list[i];

        if (p->state == READY) {
          if (current == NULL || p->arrival_time < current->arrival_time) {
            current = p;
          }
        }
      }

      if (current != NULL && current->start_time == -1) {
        current->start_time = current_time;
      }
    }
    if (current == NULL) {
      printf("Time %d: CPU Idle\n", current_time);
      Process *p = malloc(sizeof(Process));
      init_process(p, "idle", 0, 0, 0);
      execution_stack.list[current_time] = *p;
      current_time++;
      execution_stack.list =
          realloc(execution_stack.list, (current_time + 1) * sizeof(Process));
      execution_stack.count++;
      continue;
    }

    current->state = RUNNING;
    printf("Time %d: Running %s\n", current_time, current->name);

    current->remaining_time--;

    if (current->remaining_time == 0) {
      current->state = FINISHED;
      current->finish_time = current_time + 1;
    }
    execution_stack.list[current_time] = *current;
    current_time++;
    execution_stack.list =
        realloc(execution_stack.list, (current_time + 1) * sizeof(Process));
    execution_stack.count++;
  }
  return execution_stack;
}

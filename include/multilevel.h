#ifndef MULTILEVEL
#define MULTILEVEL

#include "process.h"

typedef struct {
	int *queue; // The priority queue.
	int queue_size; // To use in realloc when adding a new process to the queue.
} Level;

static int add_new_process(Level *Levels, int priority, int index);
static void check_for_new_processes(Level *Levels, ProcessList *plist, int current_time);
ProcessList schedule_multilevel(ProcessList* plist, int type, int quantum);

#endif

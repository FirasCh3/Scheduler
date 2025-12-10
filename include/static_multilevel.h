#ifndef STATIC_MULTILEVEL
#define STATIC_MULTILEVEL

#include "process.h"

struct Level {
	int *queue; // The priority queue.
	int queue_size; // To use in realloc when adding a new process to the queue.
};

static void add_new_processes(Level *Levels, ProcessList *plist, int index);
static void check_for_new_processes(Level *Levels, ProcessList *plist, int current_time);
ProcessList schedule_static_multilevel(ProcessList* plist);

#endif

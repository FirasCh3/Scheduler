#ifndef MULTILEVEL
#define MULTILEVEL

#include "process.h"

typedef struct {
	int *queue;
	int queue_size;
} Level;

static void add_new_process(Level *Levels, int priority, int index);
static void check_for_new_processes(Level *Levels, ProcessList *plist, int current_time);
static int push_to_queue_end(int* q, int size);
static void aging(Level *Levels, ProcessList *plist, int ct);
ProcessList schedule_multilevel(ProcessList* plist, int type, int quantum);

#endif

#ifndef MULTILEVEL
#define MULTILEVEL

#include "process.h"

typedef enum {
	STATIC,
	DYNAMIC
} Multilevel_type;

typedef struct {
	int *queue;
	int queue_size;
} Level;

ProcessList schedule_multilevel(ProcessList *plist, Multilevel_type m_type, int max, int quantum);

#endif

#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "process.h"

typedef struct {
	int quantum;
	int aging;
} Params;

#define sp_length 5
extern char *supported_policies[];
ProcessList scheduler(ProcessList plist, const char* policy_name, Params params);

#endif

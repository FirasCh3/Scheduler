#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "process.h"

typedef struct {
	int quantum;
	int aging;
	// Add other policy specific params here
} Params;

#define sp_length 5
extern char *supported_policies[];
ProcessList scheduler(ProcessList plist, const char* policy_name, Params params);

#endif

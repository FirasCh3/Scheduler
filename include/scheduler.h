#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "process.h"

typedef struct {
	int quantum;
	int multilevel_type;	
	// Add other policy specific params here
} Params;

#define sp_length 3
extern char *supported_policies[];
ProcessList scheduler(ProcessList plist, const char* policy_name, Params params);

#endif

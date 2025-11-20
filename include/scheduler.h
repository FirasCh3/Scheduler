#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "process.h"

ProcessList scheduler(ProcessList plist, const char* policy_name);

#endif

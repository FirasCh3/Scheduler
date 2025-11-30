#ifndef RR_H
#define RR_H

#include "process.h"

ProcessList schedule_rr(ProcessList* plist, int time_quantum);

#endif

#ifndef _RR_H
#define _RR_H

#include "process.h"

ProcessList schedule_rr(ProcessList *plist, int quantum);

#endif // _RR_H

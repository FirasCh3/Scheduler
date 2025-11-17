#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "fifo.h"

Process* schedule_fifo(ProcessList* plist)
{
    Process* selected = NULL;

    for (int i = 0; i < plist->count; i++) {
        Process* p = &plist->list[i];

        if (p->state != READY) {
            continue;
        }

        if (selected == NULL) {
            selected = p;
            continue;
        }

        if (p->arrival_time < selected->arrival_time) {
            selected = p;
        }
    }

    return selected;
}

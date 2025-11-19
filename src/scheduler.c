#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "fifo.h"
//include other policy headers as needed

void scheduler(ProcessList plist, const char* policy_name) {
    if (strcmp(policy_name, "fifo") == 0) {
        schedule_fifo(&plist);
    }
    // Add other policies here
    else {
        perror("Scheduler error: unknown policy");
        exit(EXIT_FAILURE);
    }
}

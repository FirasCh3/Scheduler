#include "scheduler.h"
#include "fifo.h"
#include "preemptive-priority.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
ProcessList scheduler(ProcessList plist, const char *policy_name) {
  if (strcmp(policy_name, "fifo") == 0) {
    return schedule_fifo(&plist);
  }
  if (strcmp(policy_name, "preemptive_priority") == 0) {
    return schedule_preemptive_priority(&plist);
  }
  // Add other policies here
  else {
    perror("Scheduler error: unknown policy");
    exit(EXIT_FAILURE);
  }
}

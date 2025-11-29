#include "scheduler.h"
#include "fifo.h"
#include "rr.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// include other policy headers as needed

ProcessList scheduler(ProcessList plist, const char *policy_name) {
  if (strcmp(policy_name, "fifo") == 0) {
    return schedule_fifo(&plist);
  } else if (strcmp(policy_name, "rr") == 0) {
		// input quantum
		int quantum = 3;
		return schedule_rr(&plist, quantum);
	}
  // Add other policies here
  else {
    perror("Scheduler error: unknown policy");
    exit(EXIT_FAILURE);
  }
}

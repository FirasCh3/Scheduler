#include "scheduler.h"
#include "fifo.h"
#include "preemptive-priority.h"
#include "process.h"
#include <rr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *supported_policies[] = {"FIFO", "Round-Robin", "Preemptive Priority"};

ProcessList scheduler(ProcessList plist, const char *policy_name, Params params) {
	size_t n = sizeof(Process) * plist.count;
	ProcessList temp = {
		.list = malloc(n),
		.count = plist.count,
	};
	memcpy(temp.list, plist.list, n);
  if (strcmp(policy_name, supported_policies[0]) == 0) {
		return schedule_fifo(&temp);
  }
	else if (strcmp(policy_name, supported_policies[1]) == 0) {
		return schedule_rr(&temp, params.quantum);
	}
	else if (strcmp(policy_name, supported_policies[2]) == 0) {
    return schedule_preemptive_priority(&temp);
  }
  // Add other policies here
  else {
    perror("Scheduler error: unknown policy");
    exit(EXIT_FAILURE);
  }
}

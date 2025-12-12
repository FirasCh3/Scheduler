#include "scheduler.h"
#include "fifo.h"
#include "preemptive-priority.h"
#include "process.h"
#include <rr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <multilevel.h>

char *supported_policies[] = {"FIFO", "Round-Robin", "Preemptive Priority", "Multilevel Static", "Multilevel Dynamic"};

  ProcessList scheduler(ProcessList plist, const char *policy_name, Params params) {
  	size_t n = sizeof(Process) * plist.count;
  	ProcessList temp = {
  		.list = malloc(n),
  		.count = plist.count,
  	};
  	memcpy(temp.list, plist.list, n);

  // For debugging   
  //ProcessList result = schedule_multilevel(&temp, 1, 2); 
  //for (int i = 0; i < result.count; i++) {
  //      printf("%s|", result.list[i].name);	
  //}

  if (strcmp(policy_name, supported_policies[0]) == 0) {
		return schedule_fifo(&temp);
  }
  else if (strcmp(policy_name, supported_policies[1]) == 0) {
	return schedule_rr(&temp, params.quantum);
  }
  else if (strcmp(policy_name, supported_policies[2]) == 0) {
    	return schedule_preemptive_priority(&temp);
  }
  else if(strcmp(policy_name, supported_policies[3]) == 0) {	
  	return schedule_multilevel(&temp, STATIC, 0, params.quantum); 
  }
  else if(strcmp(policy_name, supported_policies[4]) == 0) {	
  	return schedule_multilevel(&temp, DYNAMIC, params.aging, params.quantum); 
  }
  else {
    perror("Scheduler error: unknown policy");
    exit(EXIT_FAILURE);
  } 
}

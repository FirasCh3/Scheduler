#include "parser.h"
#include "process.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    ProcessList plist;
    plist = parse_file("config/processes.txt");
    for (int i = 0; i < plist.count; i++) {
    printf("%s %d %d %d\n", plist.list[i].name,
            plist.list[i].arrival_time, plist.list[i].burst_time,
            plist.list[i].priority);
    }

    const char* policy_name = "fifo";

    scheduler(plist, policy_name);

    free(plist.list);

    return 0;
    }

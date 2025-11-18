#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "scheduler.h"
#include "parser.h"
int main(void) {
    /*ProcessList plist;

    // this will be replaced by input parsed from config
    plist.count = 3;
    plist.list = malloc(plist.count * sizeof(Process));

    init_process(&plist.list[0], "P1", 1, 5, 1);
    init_process(&plist.list[1], "P2", 0, 3, 2);
    init_process(&plist.list[2], "P3", 2, 4, 1);


    int current_time = 0;
    const char* policy_name = "fifo"; // this will be replaced by chosen policy from menu

    while (!all_finished(&plist)) {
        update_ready_processes(&plist, current_time);

        Process* p = scheduler(plist, policy_name);
        if (p != NULL) {
            printf("Time %d: Running %s\n", current_time, p->name);
            run_process_for_one_unit(p, current_time);
        }
        else {
            printf("Time %d: CPU idle\n", current_time);
        }

        current_time++;
    }


    free(plist.list);*/
    parse_file("config/processes.txt");
    return 0;
}

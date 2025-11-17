#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

int main(void) {
    ProcessList plist;

    plist.count = 3; 
    plist.list = malloc(plist.count * sizeof(Process));

    init_process(&plist.list[0], "P1", 0, 5, 1);
    init_process(&plist.list[1], "P2", 0, 3, 2);
    init_process(&plist.list[2], "P3", 0, 4, 1);

    printf("%d\n", all_finished(&plist));

    plist.list[0].state = FINISHED;
    plist.list[1].state = FINISHED;
    plist.list[2].state = FINISHED;

    printf("%d\n", all_finished(&plist));

    free(plist.list);
    return 0;
}

#include <stdio.h>
#include "process.h"

int main(void) {
    Process p;
    init_process(&p, "Process1", 0, 10, 1);
    printf("Process Name: %s\n", p.name);
    return 0;
}
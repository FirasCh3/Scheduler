#include <stdio.h>
#include "process.h"

int main(void) {
    Process p;
    init_process(&p, "Process1", 0, 10, 1);
    return 0;
}
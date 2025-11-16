#ifndef PROCESS_H
#define PROCESS_H

typedef enum {
    NEW,
    READY,
    RUNNING,
    FINISHED
} ProcessState;

typedef struct {
    char name[32];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;

    ProcessState state;
    int start_time;
    int finish_time;
} Process;

typedef struct {
    Process* list;
    int count;
} ProcessList;

#endif

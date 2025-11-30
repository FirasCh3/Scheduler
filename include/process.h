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
    int in_queue; // to avoid duplicates (rr)
} Process;

typedef struct {
    Process* list;
    int count;
} ProcessList;

void init_process(Process* p, const char* name, int arrival, int burst, int priority);

int all_finished(ProcessList* plist);

void update_ready_processes(ProcessList* plist, int current_time);

void run_process_for_one_unit(Process* p, int current_time);

#endif

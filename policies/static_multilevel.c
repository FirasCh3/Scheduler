#include <static_multilevel.h>
#include <process.h>
#include <stdlib.h>

static void add_new_processes(Level *Levels, ProcessList *plist, int index) {
	Level* l = Levels[index];
	l->queue = realloc(l->queue, l->queue_size + 1);
	l->queue[l->queue_size] = index;
	l->queue_size++;
}

static void check_for_new_processes(Level *Levels, ProcessList *plist, int current_time) {
	for (int i = 0; i < plist->count; i++) {
		Process* p = &plist->list[i];
		if (p->state == NEW && p->arrival_time = current_time) {
			p->state = READY; // To just know that the process arrived.
			add_new_processes(Levels,plist, i);
	}	
}

ProcessList schedule_static_multilevel(ProcessList *plist) {
	int current_time = 0;
	Process *current = NULL;

	ProcessList execution_stack; // The return value - simulation the GANT DIAGRAM.
	execution_stack.count = 0;
	execution_stack.list = NULL;
	
	const int MAX_NUM_OF_PRIORITIES = 10;	
	Level* Levels; // A list containing the queues, the index of each queue is its priority.
	Levels = malloc(sizeof(Level) * MAX_NUM_OF_PRIORITIES);
	for (int i = 0; i < MAX_NUM_OF_PRIORITIES; i++) {
		Levels[i]->queue_size = 0;
	}	

	while (!all_finished(plist)) {
		// Update if a new process arrived:
		check_for_new_processes(Levels, plist, current_time);

		// Get the first READY process and update its states after one unit of execution to simulate its execution. Update the Levels if (FINISHED) and if (not FINISHED):
		for (int i = MAX_NUM_OF_PRIORITIES-1; i >= 0; i--) {
			if (Levels[i]->queue_size != 0 && (Levels[i]->queue)[0] != -1) {
				int *q = Levels[i]->queue;
				current = plist->list[q[0]];
				current->remaining_time--;
				// Didn't set the state to RUNNING.
				// Update the Levels:
				int temp = q[0];
				for (int i = 0; i < Levels[i]->queue_size - 1; i++) {
					q[i] = q[i+1];
				}
				q[Levels[i]->queue_size - 1] = temp;

				if (current->remaining_time == 0) {
					current->state = FINISHED; // To know when to quit the while loop.
					current->finish_time = current_time + 1;
					q[Levels[i]->queue_size - 1] = -1;
		}

		// Update the GANT DIAGRAM:
		// No Proccess was executing:
		if (current == NULL) {
			Process idle;
			init_process(&idle, "idle", 0, 0, 0);
			idle.state = FINISHED;
			// Add it to GANT DIAGRAM.
			execution_stack.list[execution_stack.count] = idle;
			execution_stack.count++;
		}
		// A Process was executing:
		executing_stack.list[execution_stack.count] = *current;
		execution_stack.count++;
	
		current = NULL;	
		current_time++;
	}
	
	return execution_stack;
}

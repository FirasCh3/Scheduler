#include <multilevel.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>

static int add_new_process(Level *Levels, int priority, int index) {
	if (index < 0) { return -1; }
	Level *l = &Levels[priority];
	l->queue = realloc(l->queue, l->queue_size + 1);
	l->queue[l->queue_size] = index;
	l->queue_size++;
}

static void check_for_new_processes(Level *Levels, ProcessList *plist, int current_time) {
	for (int i = 0; i < plist->count; i++) {
		Process* p = &plist->list[i];
		if (p->state == NEW && p->arrival_time == current_time) {
			p->state = READY; // To just know that the process arrived.
			add_new_process(Levels, p->priority, i);
		}
	}	
}

static void display_levels(Level *Levels, ProcessList *plist) {
	for (int i = 0; i < 10; i++) {
		if (Levels[i].queue_size != 0) {
			printf("\nPRIORITY %d\n", i);	
			int *queue = Levels[i].queue;
			for (int k = 0; k < Levels[i].queue_size; k++) {
				printf("%s|", (&plist->list[queue[k]])->name);
			}
		}
	}
}

ProcessList schedule_multilevel(ProcessList *plist, int type, int quantum) {
	int current_time = 0;
	int prec = -1;
	int exec_time = 0;
	Process *current = NULL;

	ProcessList execution_stack; // The return value - simulation the GANT DIAGRAM.
	execution_stack.count = 0;
	execution_stack.list = malloc(sizeof(Process));
	
	const int MAX_NUM_OF_PRIORITIES = 10;	
	Level* Levels; // A list containing the queues, the index of each queue is its priority.
	Levels = malloc(sizeof(Level) * MAX_NUM_OF_PRIORITIES);
	for (int i = 0; i < MAX_NUM_OF_PRIORITIES; i++) {
		Levels[i].queue = NULL;
		Levels[i].queue_size = 0;
	}	

	while (!all_finished(plist)) {
		// Update if a new process arrived:
		check_for_new_processes(Levels, plist, current_time);	

		int i = MAX_NUM_OF_PRIORITIES - 1;	
		while (Levels[i].queue_size == 0) {
			i--;
		}
		int *q = Levels[i].queue;
		current = &plist->list[q[0]];
		current->remaining_time--;
		if (q[0] != prec) { exec_time = 1; }
		else { exec_time++; }
		int j;	
		if (exec_time == quantum || current->remaining_time == 0) {
			int temp = q[0];
			j = 0;
			while (q[j+1] != -1 && j < Levels[i].queue_size -1) {
				q[j] = q[j+1];
				j++;
			}
			q[j] = temp;
		}
		if (current->remaining_time == 0) {
			current->state = FINISHED;	
			current->finish_time = current_time + 1;
			q[j] = -1;
			if (q[0] == -1) { Levels[i].queue_size = 0; }
		}  
		//add_new_process(Levels, current->priority-1, i);	
		
		display_levels(Levels, plist);
		// Update the GANT DIAGRAM:
		// No Proccess was executing:
		if (current == NULL) {
			Process *idle = &execution_stack.list[execution_stack.count];
			init_process(idle, "idle", 0, 0, 0);
			idle->state = FINISHED;
			
			execution_stack.count++;
			execution_stack.list = realloc(execution_stack.list, (execution_stack.count + 1) * sizeof(Process));
		} else {
			execution_stack.list[execution_stack.count] = *current;
			execution_stack.count++;
		}
	
		current = NULL;	
		current_time++;
	}
 	for (int i = 0; i < MAX_NUM_OF_PRIORITIES; i++) {
		free(Levels[i].queue);	
	}	
	free(Levels);	
	return execution_stack;
}

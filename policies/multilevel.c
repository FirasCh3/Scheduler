#include <multilevel.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>

static int add_new_process(Level *Levels, int priority, int index) {
	// For the dynamic priority
	if (index < 0) { return -1; }
	Level *l = &(Levels[priority]);
	l->queue = realloc(l->queue, l->queue_size + 1);
	int i = l->queue_size;
	while (l->queue[i-1] == -1) {
		l->queue[i] = -1;
		i--;
	}
	l->queue[i] = index;
	l->queue_size++;
}

static void check_for_new_processes(Level *Levels, ProcessList *plist, int current_time) {
	for (int i = 0; i < plist->count; i++) {
		Process* p = &(plist->list[i]);
		if (p->state == NEW && p->arrival_time == current_time) {
			p->state = READY;
			add_new_process(Levels, p->priority, i);
		}
	}	
}

static int push_to_queue_end(int* q, int size) {
	int temp = q[0];
	int j = 0;
	while (q[j+1] != -1 && j < size-1) {
		q[j] = q[j+1];
		j++;
	}
	q[j] = temp;

	return j;
}

static void display_levels(Level *Levels, ProcessList *plist) {
	for (int i = 0; i < 10; i++) {
		if (Levels[i].queue_size != 0) {
			printf("\nPRIORITY %d\n", i);	
			int *q = Levels[i].queue;
			for (int k = 0; k < Levels[i].queue_size; k++) {
				printf("%s|", (&plist->list[q[k]])->name);
			}
		}
	}
}

ProcessList schedule_multilevel(ProcessList *plist, int type, int quantum) {
	int current_time = 0;

	// To store the index of the process that was executing - needed to control the exec_time
	int prec = -1;

	// Tracking the exec_time so each process respect the quantum
	int exec_time = 0;

	Process *current = NULL;

	ProcessList execution_stack;
	execution_stack.count = 0;
	// Avoiding NULL here
	execution_stack.list = malloc(sizeof(Process));
	
	const int MAX_NUM_OF_PRIORITIES = 10;	

	// A dynamic array containing the queues, the index of each queue is its priority
	Level* Levels; 
	Levels = malloc(sizeof(Level) * MAX_NUM_OF_PRIORITIES);
	for (int i = 0; i < MAX_NUM_OF_PRIORITIES; i++) {
		// Sorry for the NULL in here - it could be better I know
		Levels[i].queue = NULL;
		Levels[i].queue_size = 0;
	}	

	Level* old_level;

	while (!all_finished(plist)) {
		// Update if a new process arrived
		check_for_new_processes(Levels, plist, current_time);	
		
		// Selecting the Process that need to execute
		int i = MAX_NUM_OF_PRIORITIES - 1;	
		while (Levels[i].queue_size == 0) {
			i--;
		}
		int *q = Levels[i].queue;
		// Updating current with the selected Process
		current = &(plist->list[q[0]]);
		current->remaining_time--;

		if (q[0] != prec) { 
			// Priority Preemtive
			if (prec != -1 && exec_time < quantum) {
				push_to_queue_end(old_level->queue, old_level->queue_size);	
			}
			prec = q[0];
			exec_time = 1; 
		}
		else { exec_time++; }

		// If the Process executed for a quantum or finished its executing, it will get pushed to the end of the queue
		int j;
		if (exec_time == quantum || current->remaining_time == 0) {
			j = push_to_queue_end(q, Levels[i].queue_size);
			
		}

		// If it finished, it will get deleted (-1)
		if (current->remaining_time == 0) {
			current->state = FINISHED;	
			current->finish_time = current_time + 1;
			q[j] = -1;
			// Checking if all processes finished in this queue
			if (q[0] == -1) { Levels[i].queue_size = 0; }
		}

		// I will work on the dynamic part later
		
		old_level = &(Levels[i]);

		// For debugging	
		printf("\n---------------\n");
		display_levels(Levels, plist);
		printf("\n---------------\n");

		// Update the execution_stack
		if (current == NULL) {
			Process *idle = &execution_stack.list[execution_stack.count];
			init_process(idle, "idle", 0, 0, 0);
			idle->state = FINISHED;
			
		} else {
			execution_stack.list[execution_stack.count] = *current;
		}
		execution_stack.count++;
		execution_stack.list = realloc(execution_stack.list, (execution_stack.count + 1) * sizeof(Process));
	
		current = NULL;	
		current_time++;
	}

	// Free
 	for (int i = 0; i < MAX_NUM_OF_PRIORITIES; i++) {
		free(Levels[i].queue);	
	}	
	free(Levels);	

	// Return
	return execution_stack;
}

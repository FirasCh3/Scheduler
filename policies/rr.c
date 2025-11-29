#include "process.h"
#include "rr.h"
#include <stdlib.h>
#include <string.h>

static const int CACHE_SIZE = 50; // Increment size for array extensions
struct IntList {
	int count, capacity;
	int *data;
};

// Array Helper Functions
static inline void queue_push(struct IntList *queue, int val);
static inline int queue_pop(struct IntList *queue);
static inline void list_push(ProcessList *list, int *capacity, Process p);

ProcessList schedule_rr(ProcessList *plist, int quantum) {
	int ticks = 0; // Unit measurement of time
	int exec_stack_cap = CACHE_SIZE;
	ProcessList exec_stack = {
		.list = malloc(exec_stack_cap * sizeof(Process)),
	};
	// Each element of queue is an index of process in plist, -1 for empty
	// queue length won't exceed plist->count so it probably won't need reallocation
	struct IntList queue = (struct IntList){
		.data = malloc(plist->count * sizeof(int)),
		.capacity = plist->count
	};
	memset(queue.data, -1, queue.capacity * sizeof(int));
	int curr = -1; // current executing process

	while (!all_finished(plist)) {
		// Update newly arrived processes
		for(int i = 0; i < plist->count; i++) {
			Process *p = plist->list+i;
			if (p->state == NEW && p->arrival_time == ticks) {
				p->state = READY;
				p->start_time = ticks;
				queue_push(&queue, i);
			}
		}

		// Update current process
		if (curr < 0) {
			curr = queue_pop(&queue);
			if (curr >= 0) {
				plist->list[curr].qtime_left = quantum;
				plist->list[curr].state = RUNNING;
			}
		}
		if (plist->list[curr].state == RUNNING) {
			plist->list[curr].qtime_left--;
			plist->list[curr].remaining_time--;

			if (plist->list[curr].remaining_time <= 0) {
				plist->list[curr].state = FINISHED;
				plist->list[curr].finish_time = ticks;
			} else if (plist->list[curr].qtime_left <= 0) {
				plist->list[curr].state = READY;
				queue_push(&queue, curr);
			}
		}

		// Insert process into execution stack
		if (curr < 0) list_push(&exec_stack, &exec_stack_cap, (Process){.name = "idle"});
		else {
			list_push(&exec_stack, &exec_stack_cap, plist->list[curr]);
			ProcessState state = plist->list[curr].state;
			if (state == FINISHED || state == READY) curr = -1;
		}
		ticks++;
	}

	free(queue.data);
	return exec_stack;
}

static inline void queue_push(struct IntList *queue, int val) {
	if (queue->count >= queue->capacity) {
		queue->capacity = queue->capacity + CACHE_SIZE;
		int *new_queue = realloc(queue->data, queue->capacity * sizeof(int));
		if (new_queue == NULL) {
			exit(EXIT_FAILURE);
		}
		queue->data = new_queue;
	}
	queue->data[queue->count] = val;
	queue->count++;
}

static inline int queue_pop(struct IntList *queue) {
	int temp = -1;
	if (queue->count > 0) {
		temp = queue->data[0];
		for (int i = queue->count-1; i > 0; i--) {
			queue->data[i-1] = queue->data[i];
		}
		queue->count--;
	} 
	return temp;
}

static inline void list_push(ProcessList *list, int *capacity, Process p) {
	if (list->count >= *capacity) {
		*capacity = *capacity + CACHE_SIZE;
		Process *new_list = realloc(list->list, *capacity * sizeof(Process));
		if (new_list == NULL) {
			exit(EXIT_FAILURE);
		}
		list->list = new_list;
	}
	list->list[list->count] = p;
	list->count++;
}

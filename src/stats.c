#include <process.h>
#include <stats.h>
#include <string.h>

float calculate_avg_rot(ProcessList exec_stack) {
	float res = 0.0f;
	float count = 0.0f;
	for (int i = exec_stack.count - 1; i >= 0; i--) {
		Process p = exec_stack.list[i];
		if (p.state == FINISHED && strcmp(p.name, "idle") != 0) {
			res += (float) (p.finish_time - p.start_time);
			count++;
		}
	}
	res /= count;
	return res;
}

float calculate_avg_wait(ProcessList exec_stack) {
	float res = 0.0f;
	float count = 0.0f;
	for (int i = exec_stack.count - 1; i >= 0; i--) {
		Process p = exec_stack.list[i];
		if (p.state == FINISHED && strcmp(p.name, "idle") != 0) {
			res += (float) (p.finish_time - p.arrival_time - p.burst_time);
			count++;
		}
	}
	res /= count;
	return res;
}

#include "parser.h"
#include "process.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Args parse_args(int argc, char **argv) {
	Args args = {0};
	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "-i=", 3) == 0) {
			char *start = strchrnul(argv[i], '=');
			if (start != NULL) {
				args.file_path = start + 1;
			}
		}
	}
	return args;
}

void remove_extra_whitespaces(char *str) {
  int i = 0;
  int j = 0;

  while (str[i] != '\0' && isspace(str[i])) {
    i++;
  }
  int last_char_space = 0;
  while (str[i] != '\0') {
    if (isspace(str[i])) {
      if (!last_char_space) {
        str[j] = ' ';
        j++;
        last_char_space = 1;
      }
    } else {
      str[j] = str[i];
      j++;
      last_char_space = 0;
    }
    i++;
  }
  if (j > 0 && str[j - 1] == ' ') {
    j--;
  }
  str[j] = '\0';
}
int find_whitespace(char *str, int start_index) {
  int i = start_index;
  while (str[i] != '\0' && str[i] != ' ') {
    i++;
  }
  return i;
}

int count_lines(char *path) {
  char buffer[256];
  int count = 0;
  FILE *file = fopen(path, "r");
  while (fgets(buffer, sizeof(buffer), file)) {

    if (buffer[0] != '#' && strlen(buffer) > 1) {
      count++;
    }
  }
  fclose(file);
  return count;
}

ProcessList parse_file(char *path) {
  FILE *file = fopen(path, "r");
  char buffer[256];
  if (file == NULL) {
    perror("Error opening file");
  }
  ProcessList list;
  list.count = count_lines(path);
  list.list = malloc(sizeof(Process) * list.count);
  int process_index = 0;
  rewind(file);

  while (fgets(buffer, sizeof(buffer), file)) {

    if (buffer[0] != '#' && strlen(buffer) > 1) {
      remove_extra_whitespaces(buffer);
      Process process;
      int first_whitespace_index = find_whitespace(buffer, 0);
      char *name = malloc(sizeof(char) * 32);
      strncpy(name, buffer, first_whitespace_index);
      name[first_whitespace_index] = '\0';
      int second_whitespace_index =
          find_whitespace(buffer, first_whitespace_index + 1);
      char *arrival_time =
          (char *)malloc(second_whitespace_index - first_whitespace_index);
      strncpy(arrival_time, buffer + first_whitespace_index + 1,
              second_whitespace_index - first_whitespace_index - 1);
      int third_whitespace =
          find_whitespace(buffer, second_whitespace_index + 1);
      char *burst_time =
          (char *)malloc(third_whitespace - second_whitespace_index);
      strncpy(burst_time, buffer + second_whitespace_index + 1,
              third_whitespace - second_whitespace_index - 1);
      int fourth_whitespace = find_whitespace(buffer, third_whitespace + 1);
      char *priority = (char *)malloc(fourth_whitespace - third_whitespace);

      strncpy(priority, buffer + third_whitespace + 1,
              fourth_whitespace - third_whitespace - 1);

      init_process(&process, name, atoi(arrival_time), atoi(burst_time),
                   atoi(priority));
      list.list[process_index] = process;
      free(name);
      free(arrival_time);
      free(burst_time);
      free(priority);
      process_index++;
    }
  }
  fclose(file);
  return list;
}

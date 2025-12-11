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
            char *start = strchr(argv[i], '=');
            if (start != NULL && *(start + 1) != '\0') {
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
   if (file == NULL) {
    return 0;
  }
  while (fgets(buffer, sizeof(buffer), file)) {

    if (buffer[0] != '#' && strlen(buffer) > 1) {
      count++;
    }
  }
  fclose(file);
  return count;
}

ProcessList parse_file(char *path) {
  ProcessList list = {0};

  if (path == NULL) {
    fprintf(stderr, "parse_file: NULL path\n");
    return list;
  }

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    perror("Error opening file");
    return list;
  }
  char buffer[256];
  int total_lines = count_lines(path);
  if (total_lines <= 0) {
    list.count = 0;
    list.list = NULL;
    fclose(file);
    return list;
  }
  list.count = total_lines;
  list.list = malloc(sizeof(Process) * list.count);
    if (!list.list) {
    perror("malloc");
    list.count = 0;
    fclose(file);
    return list;
  }
  int process_index = 0;
  rewind(file);

  while (fgets(buffer, sizeof(buffer), file) && process_index < list.count) {

    if (buffer[0] != '#' && strlen(buffer) > 1) {
      remove_extra_whitespaces(buffer);
      int first_whitespace_index = find_whitespace(buffer, 0);
      if (first_whitespace_index <= 0) continue;
      size_t name_len = (size_t)first_whitespace_index;
      char *name = malloc(name_len + 1);
      if (!name) break;
      strncpy(name, buffer, name_len);
      name[name_len] = '\0';

      int second_whitespace_index = find_whitespace(buffer, first_whitespace_index + 1);
      if (second_whitespace_index <= first_whitespace_index) 
        { free(name);
          continue; }
      size_t arrival_len = (size_t)(second_whitespace_index - first_whitespace_index - 1);
      char *arrival_time = malloc(arrival_len + 1);
      if (!arrival_time)
       { free(name);
         break;
         }
      strncpy(arrival_time, buffer + first_whitespace_index + 1,arrival_len);
      arrival_time[arrival_len] = '\0';


      int third_whitespace =
          find_whitespace(buffer, second_whitespace_index + 1);
      if (third_whitespace <= second_whitespace_index)
       { free(name);
         free(arrival_time);
         continue;
       }
      size_t burst_len = (size_t)(third_whitespace - second_whitespace_index - 1);
      char *burst_time = malloc(burst_len + 1);
      if (!burst_time) 
        { free(name);
          free(arrival_time); 
          break;
         }
      strncpy(burst_time, buffer + second_whitespace_index + 1,
              burst_len);
      burst_time[burst_len] = '\0';

      int fourth_whitespace = find_whitespace(buffer, third_whitespace + 1);
      if (fourth_whitespace <= third_whitespace)
        { free(name); 
          free(arrival_time); 
          free(burst_time); 
          continue; }
      size_t priority_len = (size_t)(fourth_whitespace - third_whitespace - 1);
      char *priority = malloc(priority_len + 1);
      if (!priority) 
        { free(name); 
          free(arrival_time); 
          free(burst_time); 
          break; }
      strncpy(priority, buffer + third_whitespace + 1,
              priority_len);
      priority[priority_len] = '\0';
      Process process;
      init_process(&process, name, atoi(arrival_time), atoi(burst_time),
                   atoi(priority));
      list.list[process_index] = process;
      process_index++;
      free(name);
      free(arrival_time);
      free(burst_time);
      free(priority);
    }
  }
  list.count=process_index;
  fclose(file);
  return list;
}

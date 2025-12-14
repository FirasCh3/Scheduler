#include "process.h"
#include "multilevel.h"
#ifndef PARSER_H
#define PARSER_H

typedef struct {
	char *file_path;
} Args;

ProcessList parse_file(char* path);
Args parse_args(int argc, char **argv);

#endif

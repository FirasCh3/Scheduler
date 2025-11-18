#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void remove_extra_whitespaces(char* str) {
   int i=0;
    int j=0;

    while (str[i]!='\0' && isspace(str[i])) {
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
        }else {
            str[j] = str[i];
            j++;
            last_char_space = 0;
        }
        i++;
    }
    if (j>0 && str[j-1] == ' ') {
        j--;
    }
    str[j]='\0';
}
int find_whitespace(char* str, int start_index) {
    int i = start_index;
    while (str[i]!='\0' && str[i]!=' ') {
        i++;
    }
    return i;
}
ProcessList* parse_file(char* path){
    FILE* file = fopen(path, "r");
    char buffer[256];
    if(file == NULL){
        perror("Error opening file");
    }
    ProcessList* list = malloc(sizeof(ProcessList));
    while(fgets(buffer, sizeof(buffer), file)){

        if(buffer[0]!='#' && strlen(buffer)>1){
            remove_extra_whitespaces(buffer);
            Process* process = malloc(sizeof(Process));
            int first_whitespace_index = find_whitespace(buffer, 0);
            strncpy(process->name, buffer, first_whitespace_index);
            int second_whitespace_index = find_whitespace(buffer, first_whitespace_index + 1);
            char* arrival_time = (char*) malloc(second_whitespace_index - first_whitespace_index);
            strncpy(arrival_time, buffer + first_whitespace_index + 1, second_whitespace_index - first_whitespace_index - 1);

            process->arrival_time = atoi(arrival_time);
            printf("%s %d\n",buffer , process->arrival_time);


            /*char* burst_time = "";
            strncpy(burst_time, buffer+3, 1);
            process->burst_time = atoi(burst_time);
            char* priority = "";
            strncpy(priority, buffer+4, 1);
            process->priority = atoi(priority);*/
        }
    }
    fclose(file);
    return list;


}
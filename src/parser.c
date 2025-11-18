#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
ProcessList parse_file(char* path){
    FILE* file = fopen(path, "r");
    char buffer[256];
    if(file == NULL){
        perror("Error opening file");
    }
    //ProcessList list = malloc((sizeof(ProcessList)));
    while(fgets(buffer, sizeof(buffer), file)){
        if(buffer[0]!='#'){
            int i = 0;
            while(buffer[i]!='\n'){
                i+=1;
                printf("%s", buffer);
            }
        }
    }
    fclose(file);


}
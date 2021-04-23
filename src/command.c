#include "command.h"
#include "fat.h"
#include "rprintf.h"
#include "serial.h"
#include "string.h"
#include "clibfuncs.h"
#include "parse.h"

extern char parseArguments[NUMARGS][ARGLENGTH];

command commandList[1] = {{"touch", 1, 1}};

int executeCommand(){
    int size = (sizeof(commandList)/sizeof(command));
    int i;
    for (i = 0; i < size; i++){
        if (strcmp(parseArguments[0], commandList[i].commandName) == 0){
            esp_printf((void *) putc, "%s equals %s\n", parseArguments[0], commandList[i].commandName);
        }
    }
}

void ls(char path[]){
    if (path[0] == '\0'){

    }
}

int touch(char path[]){
    int result = fatCreate(path);
    switch (result){
        case 1:
            esp_printf((void *) putc, "File already exists.\n");
            break;
        case 2:
            esp_printf((void *) putc, "Subdirectory not found.\n");
            break;
        case 3:
            esp_printf((void *) putc, "File exists in subdirectory.\n");
            break;
        case 4:
            esp_printf((void *) putc, "File could not be created.\n");
            break;
        case 5:
            esp_printf((void *) putc, "File path given is not valid\n");
            break;
        default:
            esp_printf((void *) putc, "File created.\n");
            break;
    }
    return result;
}
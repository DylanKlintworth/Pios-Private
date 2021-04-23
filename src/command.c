#include "command.h"
#include "fat.h"
#include "rprintf.h"
#include "serial.h"
void ls(char path[]){
    if (path[0] == '\0'){

    }
}

int touch(char path[]){
    int result = fatCreate(path);
    switch (result){
        case 1:
            esp_printf((void *) putc, "File already exists.\n");
        case 2:
            esp_printf((void *) putc, "Subdirectory not found.\n");
        case 3:
            esp_printf((void *) putc, "File exists in subdirectory.\n");
        case 4:
            esp_printf((void *) putc, "File could not be created.\n");
        case 5:
            esp_printf((void *) putc, "File path given is not valid\n");
        default:
            esp_printf((void *) putc, "File created.\n");
    }
    return result;
}
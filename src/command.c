#include "command.h"
#include "fat.h"
#include "rprintf.h"
#include "serial.h"
#include "string.h"
#include "clibfuncs.h"
#include "parse.h"

extern char parseArguments[NUMARGS][ARGLENGTH];
extern root_directory_entry *rootDirectoryPointers[512];

command commandList[2] = {{"touch", 1, 1}, {"ls", 1, 1}};

int executeCommand(){
    int size = sizeof(commandList)/sizeof(command);
    int i;
    for (i = 0; i < size; i++){
        if (strcmp(parseArguments[0], commandList[i].commandName) == 0){
            break;
        }
    }
    switch (i){
        case 0:
            touch(parseArguments[1]);
            break;
        case 1:
            ls(parseArguments[1]);
            break;
        default:
            return 1;
            break;
    }
    initParseArguments();
    return 0;
}

void ls(char path[]){
    int i;
    if (strcmp("/", path) == 0){
        for (i = 0; i < 512; i++){
            root_directory_entry rde = *rootDirectoryPointers[i];
            if (rde.attribute == 15){
                continue;
            } else if (rde.file_name[0] == '\0'){
                continue;
            } else {
                char *tempFilename = rde.file_name;
                char *tempExtension = rde.file_extension;
                char path[11];
                char filen[8];
                char ext[3];
                extension(filen, tempFilename, 8);
                extension(ext, tempExtension, 3);
                strncpy(path, filen, 8);
                if (ext[0] != '\0'){
                    strcat(path, ".");
                    strcat(path, ext);
                }
                if (rde.attribute == 0x10){
                    esp_printf((void *) putc, "Directory: ");
                } else if (rde.attribute == 0x20){
                    esp_printf((void *) putc, "File: ");
                }
                esp_printf((void *) putc, "%s\n", path);
            }
        }
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
            esp_printf((void *) putc, "%s created.\n", path);
            break;
    }
    return result;
}

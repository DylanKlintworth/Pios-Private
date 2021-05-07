#include "command.h"
#include "fat.h"
#include "rprintf.h"
#include "serial.h"
#include "string.h"
#include "clibfuncs.h"
#include "parse.h"
#include "environment.h"

extern char parseArguments[NUMARGS][ARGLENGTH];
extern char workingDirectory[100];
extern root_directory_entry *rootDirectoryPointers[512];

command commandList[10] = {
    {"touch"},
    {"ls"},
    {"cd"},
    {"pwd"},
    {"cat"},
    {"mkdir"},
    {"help"},
    {"echo"}
};

int executeCommand(){
    int size = sizeof(commandList)/sizeof(command);
    int i;
    short redirection = 0;
    for (i = 0; i < NUMARGS; i++){
        if (isSeparator(parseArguments[i][0], '>')){
            redirection = 1;
            break;
        }
    }
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
        case 2:
            cd(parseArguments[1]);
            break;
        case 3:
            pwd();
            break;
        case 4:
            cat(parseArguments[1]);
            break;
        case 5:
            mkdir(parseArguments[1]);
            break;
        case 6:
            help();
            break;
        case 7:
            if (redirection) {
                char buffer[strlen(parseArguments[1]) + 1];
                echo(parseArguments[1], buffer);
                file tempFile;
                int result = fatOpen(&tempFile, parseArguments[3]);
                if (result != 0){
                    esp_printf((void *) putc, "Path: %s not found.\n", parseArguments[3]);
                    nullCharArray(buffer, strlen(parseArguments[1]) + 1);
                    break;
                }
                if (tempFile.rde.attribute != 0x20){
                    esp_printf((void *) putc, "Path: %s is a directory.\n", parseArguments[3]);
                    nullCharArray(buffer, strlen(parseArguments[1]) + 1);
                    break;
                }
                esp_printf((void *) putc, "Writing to path: %s.\n", parseArguments[3]);
                fatWrite(&tempFile, buffer);
                nullCharArray(buffer, strlen(parseArguments[1]) + 1);
            } else {
                echo(parseArguments[1], "");
            }
            break;
        default:
            esp_printf((void *) putc, "\nCommand not found.\n", parseArguments[0]);
            initParseArguments();
            return 1;
            break;
    }
    redirection = 0;
    initParseArguments();
    return 0;
}

void ls(char path[]){
    int i;
    if ((strcmp("/", workingDirectory) == 0) && (strlen(path) == 0)){
        for (i = 0; i < 512; i++){
            root_directory_entry rde = *rootDirectoryPointers[i];
            if ((rde.attribute != 0x10) && (rde.attribute != 0x20)){
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
                    esp_printf((void *) putc, "File: Size - %d, ", rde.file_size);
                }
                esp_printf((void *) putc, "%s\n", path);
            }
        }
    } else {
        file tempFile;
        int result;
        if (strlen(path) == 0){
            result = fatOpen(&tempFile, workingDirectory);
        } else {
            result = fatOpen(&tempFile, path);
        }
        if (result != 0){
            esp_printf((void *) putc, "Path could not be found.\n");
            return;
        }
        if (tempFile.rde.attribute == 0x20){
            char tempFilename[11];
            rootDirectoryToFilename(tempFile.rde, tempFilename);
            esp_printf((void *) putc, "File: Size - %d, %s\n", tempFile.rde.file_size, tempFilename);
        } else if (tempFile.rde.attribute == 0x10){
            unsigned char tempEntries[2048];
            unsigned char tempChars[64][32];
            root_directory_entry *tempPointers[64];
            readFromCluster(tempEntries, tempFile.start_cluster);
            int count, index;
            count = 0; index = 0;
            for (i = 0; i < 2048; i++){
                if (i != 0 && (i % 32 == 0)){
                    count++;
                    index = 0;
                }
                tempChars[count][index] = tempEntries[i];
                index++;
            }
            for (i = 0; i < 64; i++){
                tempPointers[i] = (root_directory_entry *) tempChars[i];
            }
            for (i = 0; i < 64; i++){
                root_directory_entry rde = *tempPointers[i];
                if (rde.attribute == 15){
                    continue;
                } else if (rde.file_name[0] == '\0'){
                    continue;
                } else {
                    char tempFilename[11];
                    rootDirectoryToFilename(rde, tempFilename);
                    if (rde.attribute == 0x10){
                        esp_printf((void *) putc, "Directory: ");
                    } else if (rde.attribute == 0x20){
                        esp_printf((void *) putc, "File: Size - %d, ", rde.file_size);
                    }
                    esp_printf((void *) putc, "%s\n", tempFilename);
                }
            }
        }
    }
}

int touch(char path[]){
    int result = fatCreate(path, 0);
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

int cd(char path[]){
    if (strlen(path) == 0){
        esp_printf((void *) putc, "Changing directory to %s.\n", ROOT_DIRECTORY);
        strcpy(workingDirectory, ROOT_DIRECTORY);
        return 0;
    } else {
        int result;
        file tempFile;
        result = fatOpen(&tempFile, path);
        if (result != 0){
            esp_printf((void *) putc, "Path could not be found.\n");
            return 1;
        }
        if (tempFile.rde.attribute != 0x10){
            esp_printf((void *) putc, "Path entered is a file, not a directory.\n");
            return 2;
        }
        esp_printf((void *) putc, "Changing directory to %s.\n", path);
        strcpy(workingDirectory, path);
        return 0;
    }
}

void pwd(){
    esp_printf((void *) putc, "Working Directory: %s\n", workingDirectory);
}

int cat(char path[]){
    file tempFile;
    int result;
    result = fatOpen(&tempFile, path);
    switch (result){
        case 0:
            break;
        case 1: 
            esp_printf((void *) putc, "Directory within path could not be found\n");
            return 1;
        case 2:
            esp_printf((void *) putc, "Subdirectory within path could not be found\n");
            return 2;
        case 3:
            esp_printf((void *) putc, "Path could not be found\n");
            return 3;
    }
    if (result != 0 || tempFile.rde.attribute == 0x10){ 
        esp_printf((void *) putc, "Path is a directory.\n");
        return 4;
    }
    char buffer[tempFile.rde.file_size];
    fatRead(buffer, &tempFile, tempFile.rde.file_size);
    esp_printf((void *) putc, "%s\n", buffer);
    return 0;
}

int mkdir(char path[]){
    if (strlen(path) == 0){
        return 1;
    }
    file tempFile;
    int result = fatCreate(path, 1);
    if (result != 0){
        return 2;
    }
    result = fatOpen(&tempFile, path);
    if (result != 0){
        return 3;
    }
    unsigned char tempEntries[2048];
    unsigned char tempChars[64][32];
    root_directory_entry *tempPointers[64];
    readFromCluster(tempEntries, tempFile.rde.cluster);
    int i, index, count;
    i = 0, index = 0; count = 0;
    for (i = 0; i < 2048; i++){
        if ((i != 0) && (i % 32 == 0)){
            count++;
            index = 0;
        }
        tempChars[count][index] = tempEntries[i];
        index++;
    }
    for (i = 0; i < 64; i++){
        tempPointers[i] = (root_directory_entry *) tempChars[i];
    }
    count = 0;
    for (i = 0; i < 64; i++){
        root_directory_entry tempEntry = *tempPointers[i];
        if (tempEntry.file_name[0] == '\0'){
            break;
        }
    }
    spaceCharArray(tempPointers[i]->file_name, 8);
    spaceCharArray(tempPointers[i + 1]->file_name, 8);
    strcpy(tempPointers[i]->file_name, ".");
    strcpy(tempPointers[i + 1]->file_name, "..");
    tempPointers[i]->attribute = 0x10;
    tempPointers[i + 1]->attribute = 0x10;
    writeDataEntries(tempFile.rde, tempPointers);
    return 0;
}

int help(){
    esp_printf((void *) putc, "---------------\nHelp Menu\n---------------\nCommands:\n");
    int i;
    int size = sizeof(commandList)/sizeof(command);
    for (i = 0; i < size; i++){
        if (strlen(commandList[i].commandName) == 0){
            continue;
        }
        esp_printf((void *) putc, "\t%s\n", commandList[i].commandName);
    }
    return 0;
}

int echo(char buffer[], char redirection[]){
    if (strlen(buffer) == 0){
        esp_printf((void *) putc, "\n");
        return 0;
    }
    esp_printf((void *) putc, "%s\n", buffer);
    charArrCpy(redirection, buffer, (strlen(buffer) + 1));
    return 0;
}

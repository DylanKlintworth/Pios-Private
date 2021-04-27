#ifndef COMMAND_H
#define COMMAND_H

struct command {
    char commandName[100];
};

typedef struct command command;

int executeCommand();
void ls(char path[]);
int touch(char path[]);
int cd(char path[]);
void pwd();
int cat(char path[]);
int mkdir(char path[]);
int help();
int echo(char buffer[], char redirection[]);
#endif //PIOS_PRIVATE_COMMAND_H

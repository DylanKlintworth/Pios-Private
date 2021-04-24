#ifndef COMMAND_H
#define COMMAND_H

struct command {
    char commandName[100];
    int acceptsArguments;
    int numberArguments;
};

typedef struct command command;

int executeCommand();
void ls(char path[]);
int touch(char path[]);
int cd(char path[]);
void pwd();
void cat(char path[]);
#endif //PIOS_PRIVATE_COMMAND_H

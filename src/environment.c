#include "environment.h"
#include "clibfuncs.h"
#include "string.h"
#include "rprintf.h"

char workingDirectory[100];

void initEnvironment(){
    charArrCpy(workingDirectory, ROOT_DIRECTORY, strlen(ROOT_DIRECTORY));
}
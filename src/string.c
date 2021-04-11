#include "string.h"
#include "clibfuncs.h"
char *substring(char *dest, char *source, int start, int end){
    char *startingAddress = (source + start);
    char *endingAddress = (source + end);
    source = startingAddress;
    int index = 0;
    while ((source != endingAddress) && (*source != '\0')){
        *(dest + index) = *source;
        source++;
        index++;
    }
    *(dest + index) = '\0';
    return dest;
}

int charArrCpy(char a[], char b[], int length){
    int i;
    for (i = 0; i < length; i++){
        a[i] = b[i];
    }
    return i;
}

int charArrCpyIndex(char a[], char b[], unsigned int startIndex, unsigned int endIndex){
    int i;
    for (i = startIndex; i < endIndex; i++){
        a[i-startIndex] = b[i];
    }
    return i;
}

int charArrCpyIndexOpp(char a[], char b[], unsigned int startIndex, unsigned int endIndex){
    int i;
    for (i = startIndex; i < endIndex; i++){
        a[i] = b[i-startIndex];
    }
    return i;
}

void charArrToUpper(char a[], int length){
    int i;
    for (i = 0; i < length; i++){
        a[i] = toupper(a[i]);
    }
}

int strlenSpace(const char *str) {
    unsigned int len = 0;
    while(str[len] != ' ') {
        len++;
    }
    return len;
}

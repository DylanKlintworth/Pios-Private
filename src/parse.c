#include "parse.h"
#include "rprintf.h"
#include "clibfuncs.h"
#include "string.h"
#include "fat.h"
#include "serial.h"

char parseArguments[NUMARGS][ARGLENGTH];

void initParseArguments(){
	int i;
	for (i = 0; i < NUMARGS; i++){
		nullCharArray(parseArguments[i], ARGLENGTH);
	}
}

unsigned int numberArguments(char *buffer){
	unsigned int bufferLength = strlen(buffer);
	unsigned int count = 0;
	int i;
	for (i = 0; i <	bufferLength; i++){
		if (isSeparator(buffer[i], ' ')){
			count++;
		}
	}
	return count;
}

unsigned int longestBufferLength(char *buffer){
	unsigned int bufferLength = strlen(buffer);
	unsigned int count = 0;
	unsigned int maxLength = 0;
	int i;
	for (i = 0; i <	bufferLength; i++){
		if (isSeparator(buffer[i], ' ')){
			if (count > maxLength){
				maxLength = count;
			}
			count = 0;
			continue;
		}
		count++;
		if (i == (bufferLength - 1)){
			if (count > maxLength){
				maxLength = count;
			}
		}
	}
	return maxLength;
}

void bufferToArgs(char buffer[]){
	 unsigned int bufferLength = strlen(buffer);
	 unsigned int count, index, i;
	 unsigned int quotes = 0;
	 count = 0; index = 0;
	 for (i = 0; i < bufferLength; i++){
		 if (isSeparator(buffer[i], ' ') && (quotes == 0)){
			 count++;
			 index = 0;
			 continue;
		 } else if (isSeparator(buffer[i], '"') && (quotes == 0)){
			 quotes = 1;
			 continue;
		 } else if (isSeparator(buffer[i], '"') && (quotes == 1)){
			 quotes = 0;
			 continue;
		 }
		 parseArguments[count][index] = buffer[i];
		 index++;
	 }
}

void inputToBuffer(char *buffer, unsigned int length){
	short index = 0;
	char enteredLine = 0;
	while(!(enteredLine)){
		if (index == length){
			break;
		}
		char character = getc();
		if (!(character == '\n')){
			putc(character);
			buffer[index] = character;
			index++;
		} else {
			putc('\n');
			enteredLine = 1;
		}
	}
}

#include "list.h"
#include "delay.h"
#include "delays.h"
#include "serial.h"
#include "rprintf.h"
#include "fat.h"
#include "string.h"
#include "clibfuncs.h"
#include "parse.h"
#include "environment.h"
#include "command.h"
extern long __bss_start;
extern long __bss_end;
extern char parseArguments[NUMARGS][ARGLENGTH];
extern char workingDirectory[100];
void clear_bss();
void kernel_main() {
	initFat();
	initUart();
	initFatStructs();
	initEnvironment();
	initParseArguments();
	short isEntering = 0;
	char buffer[100];
	while (!(isEntering)){
		esp_printf((void *) putc, "%s> ", workingDirectory);
		inputToBuffer(buffer, 100);
		bufferToArgs(buffer);
		executeCommand();
		nullCharArray(buffer, 100);
	}
	while (1){
		
	}
}

void clear_bss(){
	char *begin = (char *) &__bss_start; //equivalent to &arr[0];
	char *end = (char *) &__bss_end; //equivalent to &arr[size] 
	for (; begin < end; begin++){ //set initial to beginning address of bss, while the address < end address, increment address
		*begin = 0; //dereference address and set value to 0
	}
}
	

#include "list.h"
#include "led.h"
#include "delay.h"
#include "serial.h"
#include "rprintf.h"
#include "fat.h"
#include "string.h"
#include "parse.h"
#include "environment.h"
#include "command.h"
extern long __bss_start;
extern long __bss_end;
extern char parseArguments[NUMARGS][ARGLENGTH];
void clear_bss();
void kernel_main() {
	fatInit();
	/*
	file file1, file2, file3;
	fatOpen(&file1, "/whatis/this/file.txt");
	char buffer[4000];
	fatRead(buffer, &file1, 4000);
	fatCreate("mods.txt");
	fatCreate("jazz.txt");
	fatOpen(&file2, "mods.txt");
	fatOpen(&file3, "jazz.txt");
	fatWrite(&file3, buffer);
	char buffer2[4000];
	fatRead(buffer2, &file3, 4000);
	esp_printf((void *) putc, "%s", buffer2);
	*/
	initEnvironment();
	initParseArguments();
	char *buffs = "touch /alan/alan.txt";
	bufferToArgs(buffs);
	executeCommand();
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
	

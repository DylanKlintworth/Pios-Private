#include "list.h"
#include "led.h"
#include "delay.h"
#include "serial.h"
#include "rprintf.h"
#include "fat.h"
#include "string.h"
extern long __bss_start;
extern long __bss_end;
void clear_bss();
void kernel_main() {
	clear_bss();
	fatInit();
	initFatStructs();
	fat_create("mods");
	fat_create("/alan/jazz.txt");
	file fle0, fle1;
	int c = fatOpen(&fle0, "mods");
	int d = fatOpen(&fle1, "/alan/jazz.txt");
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
	

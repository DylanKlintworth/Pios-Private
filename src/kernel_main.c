#include "list.h"
#include "led.h"
#include "delay.h"
#include "serial.h"
#include "rprintf.h"
#include "fat.h"
#include "string.h"
extern long __bss_start;
extern long __bss_end;
extern unsigned char disk[16777216];
extern fat_table_entry *fatTablePointers[8192];
void clear_bss();
void kernel_main() {
	clear_bss();
	fatInit();
	initFatStructs();
	fat_create("mods.txt");
	fat_create("jazz.txt");
	writeRootDirectory();
	writeFatTable();
	initFatStructs();
	int i;
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
	

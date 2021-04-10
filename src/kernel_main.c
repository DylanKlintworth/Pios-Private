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
	file fle0, fle1, fle3;
	char buffer0[100000];
	char buffer[11000];
	char buffer2[100];
	char buffer3[200];
	char buffer4[100];
	clear_bss();
	fatInit();
	initFatStructs();
	fat_create("mods");
	fat_create("jazz.txt");
	fatOpen(&fle1, "mods");
	fatOpen(&fle0, "jazz.txt");
	fatOpen(&fle3, "/alan/alan.txt");
	fatRead(buffer0, &fle3, 95000);
	esp_printf((void *) putc, "%s\n", buffer0);
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
	

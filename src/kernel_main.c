#include "list.h"
#include "led.h"
#include "delay.h"
#include "serial.h"
#include "rprintf.h"
#include "fat.h"
extern long __bss_start;
extern long __bss_end;
void clear_bss();
void kernel_main() {
	file fle;
	char buffer[7000];
	clear_bss();
	fatInit();
	initFatStructs();
	char *path = "/BOOT/FILE.TXT";
	fatOpen(&fle, path);
	fatRead(buffer, &fle, 50);
	for (int i = 0; i < 50; i++){
		esp_printf((void * ) putc, "%c", buffer[i]);
	}
	esp_printf((void * ) putc, "\n%d", strlen(buffer));
}

void clear_bss(){
	char *begin = (char *) &__bss_start; //equivalent to &arr[0];
	char *end = (char *) &__bss_end; //equivalent to &arr[size] 
	for (; begin < end; begin++){ //set initial to beginning address of bss, while the address < end address, increment address
		*begin = 0; //dereference address and set value to 0
	}
}
	

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
	file fle0, fle1, fle2, fle3, fle4;
	char buffer0[100000];
	char buffer[11000];
	char buffer2[100];
	char buffer3[200];
	char buffer4[100];
	clear_bss();
	fatInit();
	initFatStructs();
	char *path = "/whatis/this/file.txt";
	fatOpen(&fle0, "/alan/alan.txt");
	fatOpen(&fle1, path);
	fatOpen(&fle2, "/dylan");
	fatOpen(&fle3, "/hello.txt");
	fatOpen(&fle4, "/alan/newfile");
	fatRead(buffer0, &fle0, 100000);
	fatRead(buffer, &fle1, 11000);
	fatRead(buffer2, &fle2, 50);
	fatRead(buffer3, &fle3, 50);
	fatRead(buffer4, &fle4, 100);
	int i;
	for (i = 0; i < strlen(buffer); i++){
		esp_printf((void * ) putc, "%c", buffer[i]);
	}
	esp_printf((void * ) putc, "\n");
	for (i = 0; i < strlen(buffer2); i++){
		esp_printf((void * ) putc, "%c", buffer2[i]);
	}
	esp_printf((void * ) putc, "\n");
	for (i = 0; i < strlen(buffer3); i++){
		esp_printf((void * ) putc, "%c", buffer3[i]);
	}
	for (i = 0; i < strlen(buffer4); i++){
		esp_printf((void * ) putc, "%c", buffer4[i]);
	}
	esp_printf((void * ) putc, "\n");
	for (i = 0; i < strlen(buffer0); i++){
		esp_printf((void * ) putc, "%c", buffer0[i]);
	}
}

void clear_bss(){
	char *begin = (char *) &__bss_start; //equivalent to &arr[0];
	char *end = (char *) &__bss_end; //equivalent to &arr[size] 
	for (; begin < end; begin++){ //set initial to beginning address of bss, while the address < end address, increment address
		*begin = 0; //dereference address and set value to 0
	}
}
	

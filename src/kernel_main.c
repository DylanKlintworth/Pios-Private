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
extern root_directory_entry *rootDirectoryPointers[512];
void clear_bss();
void kernel_main() {
	clear_bss();
	fatInit();
	fat_create("mods.txt");
	fat_create("/alan/jazz.txt");
	file file1, file2, file3;
	fatOpen(&file1, "mods.txt");
	fatOpen(&file2, "/alan/jazz.txt");
	int error = fatOpen(&file3, "dylan");
	char *buffer = "wazzup";
	fatWrite(&file1, buffer, (strlen(buffer) + 1));
	char buffer1[100];
	fatRead(buffer1, &file1, 50);
	esp_printf((void *) putc, "%s", buffer1);
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
	

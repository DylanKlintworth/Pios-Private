#include "list.h"
#include "led.h"
#include "delay.h"
#include "serial.h"
#include "rprintf.h"
#include "page.h"
#include "null.h"
extern long __bss_start;
extern long __bss_end;
extern ppage *free_pages_list;
extern ppage physical_page_array[];
void clear_bss();
//unsigned long get_timer_count();
unsigned int getExecutionLevel();
void kernel_main() {

	clear_bss();
	init_pfa_list();
	ppage *initial = free_pages_list;
	esp_printf(putc, "----------Original free list----------\n");
	while (initial != NULL){
		esp_printf(putc, "Addr: %x, Next: %x, Prev: %x, Physical_Addr: %x\n", initial, initial->next, initial->prev, initial->physical_addr);
		initial = initial->next;
	}
	esp_printf(putc, "\n\n\n----------Allocated list----------\n");
	ppage *allocd_list = allocate_physical_pages(5);
	initial = allocd_list;
	while (initial != NULL){
		esp_printf(putc, "Addr: %x, Next: %x, Prev: %x, Physical_Addr: %x\n", initial, initial->next, initial->prev, initial->physical_addr);
		initial = initial->next;
	}
	esp_printf(putc, "\n\n\n----------Free list after allocating----------\n");
	initial = free_pages_list;
	while (initial != NULL){
		esp_printf(putc, "Address: %x, Next: %x, Prev: %x, Physical_Addr: %x\n", initial, initial->next, initial->prev, initial->physical_addr);
		initial = initial->next;
	}
	esp_printf(putc, "\n\n\n----------Free list after freeing----------\n");
	free_physical_pages(allocd_list);
	initial = free_pages_list;
	while (initial != NULL){
		esp_printf(putc, "Addr: %x, Next: %x, Prev: %x, Physical_Addr: %x\n", initial, initial->next, initial->prev, initial->physical_addr);
		initial = initial->next;
	}
	while(1){
	}
}

void clear_bss(){
	char *begin = (char *) &__bss_start; //equivalent to &arr[0];
	char *end = (char *) &__bss_end; //equivalent to &arr[size] 
	for (; begin < end; begin++){ //set initial to beginning address of bss, while the address < end address, increment address
		*begin = 0; //dereference address and set value to 0
	}
}

unsigned int getExecutionLevel(){
	/*unsigned int el;
	asm("mrs %0, CurrentEL"
		: "=r"(el)
		:
		:);
	return el; */
	unsigned int el;
	asm("mrs %0,CurrentEL"
		: "=r"(el)
		:
		:);
	return el;
}
	

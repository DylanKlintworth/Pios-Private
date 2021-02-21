#include <stdio.h>
#include "list.h"
#include <unistd.h>
#include "led.h"
#include "delay.h"
extern long __bss_start;
extern long __bss_end;

void clear_bss();
//unsigned long get_timer_count();

void kernel_main() {

	clear_bss();
	led_init();
	//int *mu_io_reg = 0x3F215040;
		
	while(1){
		led_on();
		delay(2);
		led_off();
		delay(2);
	}
}

void clear_bss(){
	char *begin = (char *) &__bss_start; //equivalent to &arr[0];
	char *end = (char *) &__bss_end; //equivalent to &arr[size] 
	for (; begin < end; begin++){ //set initial to beginning address of bss, while the address < end address, increment address
		*begin = 0; //dereference address and set value to 0
	}
}

/*unsigned long get_timer_count() {
	unsigned long *timer_count_register = 0x3f003004;
	return *timer_count_register;
} */

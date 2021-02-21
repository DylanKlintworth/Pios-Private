#ifndef LED_H
	#define LED_H
	#define GPFSEL0 0xFE200000
	#define GPFSEL1 0xFE200004
	#define GPFSEL2 0xFE200008
	#define GPFSEL3 0xFE20000C	
	#define GPFSEL4 0xFE200010
	#define GPSET0 0xFE20001C
	#define GPSET1 0xFE200020
	#define GPCLR0 0xFE200028
	#define GPCLR1 0xFE20002C
	void led_init();
	void led_on();
	void led_off();
#endif

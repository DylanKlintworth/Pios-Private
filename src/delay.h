#include <stdint.h>
#ifndef DELAY_H
	#define DELAY_H
	#define CLO 0xFE003004 //found in peripheral datasheet https://datasheets.raspberrypi.org/bcm2711/bcm2711-peripherals.pdf#page=141
	//system timer register incorrect on Professor Klingensmith's systimer.pdf
	void delay(uint32_t seconds);
#endif

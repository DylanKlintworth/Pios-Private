#include "delay.h"
//making delay based on Professor Klingensmith's systimer.pdf
//CLO register time is in microseconds
void delay(uint32_t seconds) {
    uint32_t* clo = (uint32_t*) CLO; //get CLO; defined in delay.h
    volatile uint32_t currentTime = *clo; //changes value so declare volatile; gets time in microseconds
    uint32_t finishTime = currentTime + (seconds * 1000000); //microseconds = seconds * 1E6
    while(currentTime < finishTime){
	currentTime = *clo;
	asm("nop");
    }
}

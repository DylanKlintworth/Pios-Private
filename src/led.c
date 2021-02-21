#include "led.h" //defines prototypes and gpio registers
#include <stdint.h> //allows for precise 32bit integers
/*
 * -----------------Bitmasking Info---------------------
 * 	clear ith bit operation == input = input & ~(1 << ith bit)
 * 	set ith bit operation == input = input | (i << ith bit)
 */
void led_init() {
	
	uint32_t *gpfsel4 = (uint32_t *) GPFSEL4; //get gpfsel4 register

	/* function select 42 bits 8:6
	*bitmask to zero bits 8:6 is
	*  1111 1111 1111 1111 1111 1110 0011 1111
	*0x F    F    F    F    F    E    3    F
	*/

	*gpfsel4 = *gpfsel4 & 0xFFFFFE3F; //zero out bits 6-8 at gpfsel4

	/* configure 8:6 as 001 to set gpio 42 as general purpose output 
	set bit 6 to 1 = (1 << 6);
	gpfsel: 44  43  42  41  40
	bits: 0-000-000-001-000-000
	*/ 

	*gpfsel4 = *gpfsel4 | (1 << 6); //bit 6 set to 1

}

void led_on(){
	
	uint32_t *gpset1 = (uint32_t *) GPSET1; //get gpset1 register
	
	/*
	 * bits 0:25 set register; n=32..57 (1 bits per register) 
	 * set register 42 
	 *  42-41-40-39-38-37-36-35-34-33-32
	 *  1  0  0  0  0  0  0  0  0  0  0 == (1 << 10)
	 */
	
	*gpset1 = *gpset1 | (1 << 10); //bit 10 (GPIO 42) set to on
}

void led_off(){
	
	uint32_t *gpclr1 = (uint32_t *) GPCLR1; //get gpclr1 register
	
	/*
	 * bits 0:25 set register; n=32..57 (1 bits per register) 
	 * set register 42 
	 *  42-41-40-39-38-37-36-35-34-33-32
	 *  1  0  0  0  0  0  0  0  0  0  0 == (1 << 10)
	 */
	
	*gpclr1 = *gpclr1 | (1 << 10);

}
/* Professor Klingensmith's delay -- not currently working
void delay(unsigned int seconds){
	unsigned int i, j;
	unsigned int d = 0xff;
	for (i = 0; i < 0x7f; i++){
		for (j = 0; j < (d * seconds); j++){
			asm("nop");
		}
	}
}
*/

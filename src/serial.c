#include <stdint.h>
#include "serial.h"
#include "gpio.h"
#include "delays.h"
 /*
 * Copyright (C) 2018 bzt (bztsrc@github)
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 */
void putc(int data){
	do {
		asm volatile("nop");
	} while(!(*AUX_MU_LSR&0x20));
	*AUX_MU_IO = data;
}

char getc(){
	char r;
	do{
		asm volatile("nop");
	}while(!(*AUX_MU_LSR&0x01));
	r = (char)(*AUX_MU_IO);
	return r=='\r'?'\n':r;
}

void initUart(){
    register unsigned int r;
    *AUX_ENABLE |=1;
    *AUX_MU_CNTL = 0;
    *AUX_MU_LCR = 3;
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 0xc6;
    *AUX_MU_BAUD = 270;
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15));
    r|=(2<<12)|(2<<15);
    *GPFSEL1 = r;
    *GPPUD = 0;
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;
    *AUX_MU_CNTL = 3;
}

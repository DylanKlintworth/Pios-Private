#ifndef SERIAL_H
	#define SERIAL_H
	#include <stdint.h>
	#include "gpio.h"
	#define RPI3_MU_IO 0x3F215040
	#define RPI4_MU_IO 0xFE215040
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
	#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
	#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
	#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
	#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
	#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
	#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
	#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
	#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
	#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
	#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
	#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
	#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))
	void initUart();
	void putc(int data);
	char getc();
#endif

#ifndef SERIAL_H
	#define SERIAL_H
	#define RPI3_MU_IO 0x3F215040
	#define RPI4_MU_IO 0xFE215040
	void putc(int data);
	void getc(char buffer[]);
#endif

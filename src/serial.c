#include <stdint.h>
#include "serial.h"

void putc(int data){
	uint32_t *mu_io	= (uint32_t *) RPI3_MU_IO;
	*mu_io = data;
}

void getc(char buffer[]){
	uint32_t *mu_io	= (uint32_t *) RPI3_MU_IO;
	buffer[0] = *mu_io;
}
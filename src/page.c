#include "page.h"

ppage physical_page_array[128];
ppage *free_list = NULL;

void init_pfa_list(void){
	

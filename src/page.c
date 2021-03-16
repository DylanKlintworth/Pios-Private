#include "page.h"
#include "null.h"
#define NUMBERPAGES 128

extern 

void ppageAdd(ppage **head, ppage *newElement){
    ppage *iterator = (ppage *) head;

    while (iterator->next != NULL){
        iterator = iterator->next;
    }

    iterator->next = newElement;
    newElement->prev = iterator;

}

void ppageRemove(ppage *b){
    if(b->next != NULL){
		b->next->prev = b->prev;
    }
	b->prev->next = b->next;
	b->next = NULL;
	b->prev = NULL;
}


ppage physical_page_array[NUMBERPAGES]; //128 pages of 2MB each = 256MB of pages
ppage *free_pages_list = NULL; //Initial free page list

void init_pfa_list(void){

    void *pages_address = 0x200000; //0x200000 is 2MB (16^5); first physical address
    //create head for free_pages_list - initial allocation
    int i;
    for (i = 1; i < NUMBERPAGES; i++){
        ppageAdd(&free_pages_list[0], (ppage *) &physical_page_array[i]);
        physical_page_array[i].physical_addr = pages_address;
        pages_address = pages_address + 0x200000;
    }
}

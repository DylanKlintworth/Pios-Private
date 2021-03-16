#include "page.h"
#include "null.h"
#define NUMBERPAGES 128

extern 

void ppageAdd(ppage **head, ppage *newElement){

    ppage *iterator = (ppage*) head;

    newElement->next = iterator->next;
    newElement->prev = iterator;

    iterator->next = newElement;
    
    if(newElement->next != NULL){
        newElement->next->prev = newElement;
    }
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
    for (i = 0; i < NUMBERPAGES; i++){
        physical_page_array[i].physical_addr = pages_address; //give page physical address equal to pages_address (multiple of 2MB)
        ppageAdd(free_pages_list, &physical_page_array[i]); //add physical page to free pages linkedlist
        pages_address = pages_address + 0x200000; //increment pages address by 2MB
    }
}

void free_physical_pages(ppage *ppage_list){
    
}

ppage *allocate_physical_pages(unsigned int npages){
    ppage *allocd_list = NULL; //list of allocated pages to be returned
    ppage *initial = free_pages_list; //start at initial node of free list
    int i = 0;
    while (i < npages){ //remove and allocated npages
        ppageRemove(initial); //remove node from free_pages_list (next & previous removed)
        ppageAdd(allocd_list, initial); //add node to allocd_list
        i++;
    }
    return allocd_list; //return new list of physical pages
}
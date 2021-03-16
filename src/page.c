#include "page.h"
#include "null.h"
#define NUMBERPAGES 128

void ppageAdd(ppage *head, ppage *newElement){

    ppage *iterator = (ppage *) head;

    // Link element b into the list between iterator and iterator->next.
    
    while(iterator->next != NULL){
        iterator = iterator->next;
    }
    iterator->next = newElement;
    newElement->prev = iterator;
}

ppage* ppageRemove(ppage *b){

    ppage *iterator = b;
    
    while(iterator->next != NULL){
        iterator = iterator->next;
    }
    iterator = iterator->prev;
    iterator->next->prev = NULL;
    ppage *temp = iterator->next;
    iterator->next = NULL;
	return temp;
}


ppage physical_page_array[NUMBERPAGES]; //128 pages of 2MB each = 256MB of pages
ppage *free_pages_list = &physical_page_array[0]; //Initial free page list

void init_pfa_list(void){
    free_pages_list = &physical_page_array[0]; //free_pages_list needs pointer to start list
    void *pages_address = (void *) 0x200000; //0x200000 is 2MB (16^5); first physical address
    physical_page_array[0].physical_addr = pages_address;
    pages_address += 0x200000;
    //create head for free_pages_list - initial allocation
    int i;
    for (i = 1; i < NUMBERPAGES; i++){
        physical_page_array[i].physical_addr = pages_address; //give page physical address equal to pages_address (multiple of 2MB)
        ppageAdd(free_pages_list, &physical_page_array[i]); //add physical page to free pages linkedlist
        pages_address = pages_address + 0x200000; //increment pages address by 2MB
    }
}

void free_physical_pages(ppage *ppage_list){
    ppageAdd(free_pages_list, ppage_list); //adds ppage list to end of free_page_list
}

ppage *allocate_physical_pages(unsigned int npages){
    ppage *allocd_list; //list of allocated pages to be returned
    ppage *pgs[npages];

    int i = 0;
    while (i < npages){
        pgs[i] = ppageRemove(free_pages_list);
        i++;
    }
    int j;
    allocd_list = pgs[0];
    for (j = 1; j < npages; j++){
        ppageAdd(allocd_list, pgs[j]);
    }

    return allocd_list; //return new list of physical pages
}
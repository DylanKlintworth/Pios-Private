#include <stdio.h>
#include "list.h"

int main(){
	LinkedList d = {NULL, NULL, 99};
	LinkedList c = {NULL, NULL, 10};
	LinkedList b = {NULL, NULL, 22};
	LinkedList a = {NULL, NULL, 57};
	list_add(&a, &b);
	list_add(&a, &c);
	list_add(&a, &d);
	list_remove(&a, &c);
	list_add(&a, &c);
	list_remove(&a, &d);
	return 0;
}

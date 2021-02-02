#ifndef List_H
#define List_H

struct LinkedList {
	struct LinkedList *next;
	struct LinkedList *previous;
	int data;
};

typedef struct LinkedList LinkedList;

void list_add(LinkedList *headnode, LinkedList *newNode);

void list_remove(LinkedList *headnode, LinkedList *removeNode);

#endif

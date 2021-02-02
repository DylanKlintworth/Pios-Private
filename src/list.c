#include <stdio.h>
#include "list.h"

void list_add(LinkedList *headNode, LinkedList *newNode){
	LinkedList *nodeAddress = headNode; //address of head node
	while (nodeAddress->next != NULL){ //while the next node isn't null, assign node index to next node
		nodeAddress = nodeAddress->next;
	}
	nodeAddress->next = newNode; //set next node to the new nodes address when found to be null
	newNode->previous = nodeAddress; //
}	

void list_remove(LinkedList *headNode, LinkedList *removeNode){
	LinkedList *rn = removeNode;
	removeNode->next->previous = removeNode->previous;
	removeNode->previous = NULL;
	LinkedList *hn = headNode;
	while (hn->next != NULL){
		if (hn->next == rn){
			LinkedList *tempNode = hn->next->next;
			hn->next = tempNode;
			break;
		}
		hn = hn->next;
	}
}

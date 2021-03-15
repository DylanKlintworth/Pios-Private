#ifndef __LIST_H
    #define __LIST_H
    struct listElement {
        struct listElement *next;
        struct listElement *prev;
    };

    typedef struct listElement listElement;

    void listAdd(listElement **head, listElement *newElement);
    void listRemove(listElement *b);
#endif
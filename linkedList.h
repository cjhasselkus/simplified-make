#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct node 
{ 
    char * string;
    struct node * next;
   
} node; 

node * createNode(char * string);
void printList(node * head);
node * addToEnd(node ** head, char * newString);
void freeList(node * head);

#endif // LINKEDLIST_H_
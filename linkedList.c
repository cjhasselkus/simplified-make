#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"
#include "buildSpec.h"

node * createNode(char * string)
{
    //allocate space for the struct
    node * nodeStruct = malloc(sizeof(node));
    if(!nodeStruct)
    {
        fprintf(stderr, "Error: Memory allocation failed.");
        exit(1);
    }
    
    //state variables for this queue
    nodeStruct->string = string;
    nodeStruct->next = NULL;
    
    return nodeStruct;
}

void printList(node * head)
{
    node * n = head;
    while(n != NULL)
    {
        if(n->string == NULL)
        {
            //this is the end of the command line
            printf("\n");
        }
        else
        {
            printf("%s ", n->string);
        }
        n = n->next;
    }
}

node * addToEnd(node ** head, char * newString)
{
    node * newNode = createNode(newString);
    node *last = *head;
    
    if(*head == NULL)
    {
        *head = newNode;
        return newNode; 
    }
    else
    {
        while(last -> next != NULL) //while we are not at the final node
        {
            last = last->next;
        }
        
        last->next = newNode;
        return newNode;
    }

}

void freeList(node * head)
{
    node* tmp;

    while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp->string);
       tmp->string = NULL;
       free(tmp);
       tmp = NULL;
    }
    
    return;
}
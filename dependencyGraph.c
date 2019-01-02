#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "linkedList.h"
#include "dependencyGraph.h" 
#include "commandExec.h"
#include "buildSpec.h"

//structures
graphNodeList * createGraphNodeList(graphNode * target)
{
    //allocate space for the struct
    graphNodeList * graphNodeListStruct = malloc(sizeof(graphNodeList));
    if(!graphNodeListStruct)
    {
        fprintf(stderr, "Error: Memory allocation failed.");
        exit(1);
    }
    
    
    //state variables for this queue
    graphNodeListStruct->targetPointer = target;
    graphNodeListStruct->next = NULL;
    return graphNodeListStruct;
}


//functions
void printGraph(graphNode * head)
{
    //a secondary print function that was used for testing
    /*node * n = (node*)head->dependencies;
    printf("%s \n", head->name);
    while(n != NULL)
    {
        if(n->string == NULL)
        {
            //this is the end of the line
        }
        else
        {
            printf("%s ", n->string);
        }
        n = n->next;
    }
    printf("\n");*/
    
    graphNode * n = (graphNode*)head;
    printf("%s \n", head->name);
    while(n != NULL)
    {
        if(n->name == NULL)
        {
            //this is the end of the line
        }
        else
        {
            printf("%s ", n->name);
        }
        n = n->next;
    }
    printf("\n");
    
}

void printTargetDependencies(graphNodeList * head)
{
    graphNodeList * n = (graphNodeList*)head;
    while(n != NULL)
    {
        if((n->targetPointer)->name == NULL)
        {
            //this is the end of the line
        }
        else
        {
            printf("%s", (n->targetPointer)->name);
        }
        n = n->next;
    }
    printf("\n");
}


graphNode * addToGraph(graphNode ** head, graphNode * newNodePointer)
{
    graphNode * newNode = newNodePointer;
    graphNode *last = *head;
    
    if(*head == NULL)
    {
        *head = newNode;
        return newNode;
    } 
    else
    {
        while(last->next != NULL) //while we are not at the final node
        {
            if(strcmp(last->name, newNode->name) == 0 || strcmp(last->next->name, newNode->name)==0 ) //if this target name is already in the graph
            {
                fprintf(stderr, "Error: Target %s has multiple build specifications.\n", newNode->name);
                exit(1);
            }
            last = last->next;
        }
        
        last->next = newNode;
        return newNode;
    }
}

int addToGraphNodeList(graphNodeList ** head, graphNode * newNode)
{
    graphNodeList * newMember = createGraphNodeList(newNode);
    
    graphNodeList * last = *head;
    
    if(*head == NULL)
    {
        *head =  newMember;
        return 0;
    }
    else
    {
        while(last -> next != NULL) //while we are not at the final node
        {
            last = last->next;
        }
        last->next = newMember;
        return 0;
    } 
}


int total = 0;
graphNode * addEdges(graphNode **head) 
{
    graphNode *lastPre = *head;
    total = 0;
    if(head == NULL)
    {
       //graph empty
       return NULL;
    }
    else
    {
        while(lastPre != NULL)
        {
            node * lastDep = lastPre ->dependencies->next;
            total++;
            while(lastDep != NULL)//for each dependency of this target
            {
                total++;
                int matchFound = 0;
                graphNode *lastPost = *head;
                while(lastPost != NULL)
                { 
                    if(strcmp(lastPost->name,lastDep->string) == 0)
                    {
                       addToGraphNodeList(&lastPre->dependsOn, lastPost);
                       matchFound = 1;
                    }
                    lastPost = lastPost->next;
                }
                //if you found no matching dependency
                if(matchFound == 0)
                {
                    //check if this is a file in your directory
                    if(access(lastDep->string, F_OK) != -1)
                    {
                        //file exists
                    }
                    else
                    {
                        fprintf(stderr, "Error: No matching target or file for dependency %s.\n", lastDep->string);
                        exit(1);
                    }
                }
                lastDep = lastDep->next;                
            }
            if(lastPre->dependsOn != NULL)
            {
               //printTargetDependencies(lastPre->dependsOn);
            }
            lastPre = lastPre->next;
        }
    }
    return *head;
}

void traverse(graphNode * start)
{

    if(start == NULL)
    {
        return;
    }
    //create stack
    int top = -1;
    int top2 = -1;
    int numItems = 0;
    int numItems2 = 0;
    graphNode * stack[total]; 
    graphNode * stack2[total];
    
    //push node onto stack 1
    stack[++top] = start; 
    numItems++;
    
    while(numItems>0) //while stack is non empty
    {
        //pop from stack
        graphNode * n = stack[top];
        top = top-1;
        numItems--;
        
        //push onto second stack
        stack2[++top2] = n;
        numItems2++;
        
        //push children into stack
        graphNodeList * child = (graphNodeList*)n->dependsOn;
        while(child != NULL)
        {
            if(child->targetPointer->name == NULL)
            {
                //this is the end of the line
            }
            else
            {                    
                //push this node onto the stack 
                stack[++top] = child->targetPointer; 
                numItems++;
            }
            child = child->next;
        }
    }
    
    int dependencyRan = 0;
    //print contents of second stack
    while(numItems2 > 0)
    {
        graphNode * popped = stack2[top2];
        top2 = top2-1;
        numItems2--;
        
        
        //check if this target's commands need to be run i.e. is it out of date?
        int needToRun = 0;
        if(dependencyRan == 1)//if one of this target's dependencies had to run, so does this target
        {
            needToRun = 1;
        }
        else
        {
            node * n =(node*)popped->dependencies->next;
            int dependencyCounter = 0;
            if(access(popped->name, F_OK) != -1)//if target is a filename
            {
                //file exists
                struct stat attrTarget;
                stat(popped->name, &attrTarget);
                
                while(n != NULL)
                {
                    dependencyCounter++;
                    //check if this is a file in the directory
                    if(access(n->string, F_OK) != -1)
                    {
                        //file exists
                        struct stat attrDepend;
                        stat(n->string, &attrDepend);  
                        
                        if((attrDepend.st_mtime - attrTarget.st_mtime) > 0)//if dependency is newer
                        {
                           needToRun = 1;
                        }
                    }
                    n = n->next;
                }

            }
            else //if the target is something like "clean" or a .o that hasn't been made yet 
            {
                needToRun = 1;
            }
            
            if(dependencyCounter == 0)
            {
                needToRun = 1;
            }
        }
        
        //check for filename dependencies
        //if file mod time is more recent than the target filename (or if the target filename doesn't exist yet), then we have to run target
        //if one of the lower dependencies was run already that means we also have to run 
        if(needToRun == 1)
        {
            runTarget(popped);
            dependencyRan = 1;
        }
    }
}


int containsCycles(graphNode * graphStart)
{
    //all nodes already start as being unvisited
    graphNode * n = (graphNode*)graphStart;
    while(n != NULL)
    {
        if(n->visited == 0)//unvisited
        {
            if(DFSCycleCheck(n) == 1)//if DFS cycle check returns true 
            {
                fprintf(stderr, "Error: Cycle in makefile involving target %s.\n", n->name);
                exit(1); //CYCLE
            }
        }
        n = n->next;
    }
    
    return 0; //0 meaning there isn't a cycle and the program can continue running
}



int DFSCycleCheck(graphNode * node) //Recursive 
{
    node->visited = 1;//set visited to be 1 i.e. in the DFS stack
    
    //for each of this node's dependencies
    graphNodeList * n = (graphNodeList*)node->dependsOn;
    while(n != NULL)
    {
        if((n->targetPointer)->name == NULL)
        {
            //this is the end of the line
        }
        else
        {
            if(n->targetPointer->visited == 1)//if this node is already in the stack
            {
                return 1;//CYCLE
            }
            
            if(n->targetPointer->visited == 0 && DFSCycleCheck(n->targetPointer))
            {
                 return 1;//CYCLE
            }
        }
        n = n->next;
    }
    
    node->visited = 2;//this node has now been visisted (2)
    return 0; //no cycle here 
}
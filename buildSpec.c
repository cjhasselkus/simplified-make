#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"
#include "buildSpec.h"
#include "dependencyGraph.h"


graphNode * graph = NULL; //accessed with getGraph()
graphNode * createBuildSpec(node * target, node *command)
{
    //allocate space for the struct
    graphNode * buildSpecStruct = malloc(sizeof(graphNode));
    if(!buildSpecStruct)
    {
        fprintf(stderr, "Error: Memory allocation failed.");
        exit(1);
    }
    
    buildSpecStruct->name = target->string; 
    buildSpecStruct->visited = 0; //for cycle checking - 0 = unvisited, 1 = in the stack, 2 = visited
    buildSpecStruct->dependencies = target;
    buildSpecStruct->commands = command;
    buildSpecStruct->dependsOn = NULL;
    buildSpecStruct->next = NULL;
    
    return buildSpecStruct;
    
}
 
//add buildSpec to the graph
graphNode * addBuildSpecToGraph(graphNode * targetSpec)
{
    graphNode* targetGraphNode = addToGraph(&graph, targetSpec);
    return targetGraphNode;
}

graphNode * getGraph()
{
    return graph;
}

void freeMemory(graphNode * spec)
{    
    graphNodeList * tmpGNL;
    graphNode * tmp;
    
    while(spec != NULL)
    {
        tmp = spec;        
        //free graphNodeList of dependencies
        //graphNodeList * headGNL = spec->dependsOn;
        while(spec->dependsOn != NULL)
        {
            tmpGNL = spec->dependsOn;
            spec->dependsOn = spec->dependsOn->next;
            free(tmpGNL);
            tmpGNL = NULL;
        }
        
        
        //free commands and targets  
        if(spec->commands != NULL)
        {
            freeList(spec->commands);
        }
        
        if(spec->dependencies != NULL)
        {
            freeList(spec->dependencies);
        }
        
        spec = spec->next;
        free(tmp);
        tmp = NULL;
    }
    return;  
}
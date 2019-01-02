
#ifndef DEPENDENCYGRAPH_H_
#define DEPENDENCYGRAPH_H_
#include "linkedList.h"

typedef struct graphNode 
{ 
    int visited;
    char * name; //name of this dependency (or target) (will be a filename or another dependency)
    node * dependencies; //list of dependencies
    node * commands; //list of dependencies
    struct graphNode * next; //pointer to the next target
    struct graphNodeList * dependsOn; //list of other targets that this node depends on (edges)
   
} graphNode; 

typedef struct graphNodeList
{ 
    struct graphNode * targetPointer; //list of other targets that this node depends on (edges)
    struct graphNodeList * next;
} graphNodeList; 


//create a graph node
graphNode * createGraphNode(char * string, int target, node * dependencies, node * commands);

//print a subset of the nodes in a graph
void printGraph(graphNode * head);

//print a node's dependencies
void printTargetDependencies(graphNodeList * head);

//add a new node to the graph    
graphNode * addToGraph(graphNode ** head, graphNode * newNodePointer);

//add a node to a list of dependencies
int addToGraphNodeList(graphNodeList ** head, graphNode * newNode);

//add edges to the graph
graphNode * addEdges(graphNode **head);

//traverse the graph using post-order traversal
void traverse(graphNode * start);

//check if the graph contains cycles
int containsCycles(graphNode * graphStart);
int DFSCycleCheck(graphNode * node);

#endif // DEPENDENCYGRAPH_H_
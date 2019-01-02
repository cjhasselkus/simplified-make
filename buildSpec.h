#ifndef BUILDSPEC_H_
#define BUILDSPEC_H_
#include "dependencyGraph.h"

typedef struct buildSpec 
{ 
    struct node * targetList;
    struct node * commandList;
   
} buildSpec; 

graphNode * createBuildSpec(node * target, node *command);
graphNode * addBuildSpecToGraph(graphNode * targetSpec);
graphNode * getGraph();
void freeMemory(graphNode * spec);

#endif // BUILDSPEC_H_
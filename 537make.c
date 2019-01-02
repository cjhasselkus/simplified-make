#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "dependencyGraph.h"
#include "commandExec.h"
#include "fileParser.h"
#include "linkedList.h"
#include "buildSpec.h"

const char * makefile = "makefile";
const char * Makefile = "Makefile";

int main(int argc, char * argv[])
{
    //get the target name that should be run
    char * targetName;   
    if(argc == 1)
    {
        //just run using traverse(graph)
        targetName = NULL;
    }
    else
    {
        //you will run using the target that matches this name if there is one
        targetName = argv[1];
    }
    
    //find the makefile
    FILE *makefilePointer;   
    graphNode * graph = NULL;   
    
    //if "makefile" is used
    if((makefilePointer = fopen(makefile, "r")) != NULL)
    {
        //parse the file into buildSpecs that form a graph
        if(parseFile(makefilePointer) == 1)
        {
            return 1;
        } 
        fclose(makefilePointer);
        
        //get the graph
        graph = getGraph();
        
        //add edges to the graph
        graph = addEdges(&graph); 
        
        //check if the graph contains cycles, and if it does, terminate
        containsCycles(graph);    
        
        //find the target you requested by the commandline (if it exists)     
        graphNode * target = findProcess(targetName, graph);
        
        //using post order traversal, traverse the graph and build necessary targets in this order
        traverse(target); 
        
        freeMemory(graph);
    }
    
    //if "Makefile" is used
    else if((makefilePointer = fopen(Makefile, "r")) != NULL)
    {
        //parse the file into buildSpecs that form a graph
        if(parseFile(makefilePointer) == 1)
        {
            return 1;
        } 
        fclose(makefilePointer);
        
        //get the graph
        graph = getGraph();
        
        //add edges to the graph
        graph = addEdges(&graph); 
        
        //check if the graph contains cycles, and if it does, terminate
        containsCycles(graph);    
        
        //find the target you requested by the commandline (if it exists)     
        graphNode * target = findProcess(targetName, graph);
        
        //using post order traversal, traverse the graph and build necessary targets in this order
        traverse(target); 
        
        freeMemory(graph);
    }
    else
    {
        fprintf(stderr, "Invalid makefile: No file named makefile/Makefile found.\n"); 
        return 1;
    }
    
    return 0;
}
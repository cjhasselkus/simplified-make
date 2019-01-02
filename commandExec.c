#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "dependencyGraph.h"
#include "commandExec.h"
#include "fileParser.h"
#include "linkedList.h"
#include <sys/wait.h>

int runTarget(graphNode * target)
{
    //run each command for this target using runCommand function
    int newCommand = 1;
    node * n = (node*)target->commands;
    while(n != NULL)
    {
        if(n->string == NULL)
        {
            //this is the end of the line
            //a new command will start after this one
            newCommand = 1;
        }
        else
        {
            if(newCommand == 1)
            {
                //if this the start of a new command, run it!
                runCommand(n);
                newCommand = 0;
            }
        }
        n = n->next;
    }
    return 0;
}


//locate the process matching a target name
graphNode * findProcess(char * processName, graphNode * graphTop)
{    
    if(processName == NULL)
    {
        return graphTop; //run the target listed first in the makefile!
    }
    
    graphNode * n = graphTop;
    while(n != NULL)
    {
        if(n->name == NULL)
        {
            //this is the end of the line
        }
        else
        {
            if(strcmp(n->name, processName) == 0)//find the process that matches
            {
                return n;
            }
        }
        n = n->next;
    }
    
    //if no processes match
    fprintf(stderr, "Error: Makefile doesn't contain a rule for the command line argument %s.\n", processName); 
    exit(1);
    return NULL;

}


//given a command, call fork and exec to run the command
int runCommand(node * command )
{
    //print comand
    //first put the args into an array to pass to execvp 
    node * n = command;
    int numArgs = 0;
    while(n != NULL)
    {
        if(n->string == NULL)
        {
            //this is the end of the line
            break;
        }
        else
        {
            printf("%s ", n->string);
            numArgs++;
        }
        n = n->next;
    }
    printf("\n");
    
    
    //put the arguments into args and the command into cmd
    char * args[numArgs];
    char * cmd;
    
    int counter = 0;
    n = command;
    while(n != NULL)
    {
        if(n->string == NULL)
        {
            //this is the end of the line
            break;
        }
        else
        {
            if(counter == 0)
            {
                cmd = n->string;
            }
            args[counter] = n->string;

        }
        n = n->next;
        counter++;
    }
    args[counter] = NULL; //add a null at the end of the arguments to pass to execvp
    
    
    //fork
	  int childPID = fork();
    int childStatus;
	  if(childPID == 0)
    {
        //exec
        if (execvp(cmd, args) < 0) //if an error is returned by execvp (-1)
        {
            fprintf(stderr, "Error: execvp failed on command ");
            printList(command);
            printf("\n");
            exit(1);
        }
	  }
    else
    {
        //wait for the child process to finish
    	  if(wait(&childStatus) < 0)//command failed
        {
             fprintf(stderr, "Error: child process failed using command ");
             printList(command);
             printf("\n");
             exit(1);
        }
    }
     return 0; 
}
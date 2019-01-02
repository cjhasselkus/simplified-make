#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fileParser.h"
#include "buildSpec.h"
#include "linkedList.h"


const int BUFSIZE = 1024;

int parseFile(FILE * makeFile)
{
    int notEOF = 1;    
    int lineCounter = 0;
    int hasTarget = 0;
    
    graphNode * currentBuildSpec = NULL;
    while(notEOF)
    {
        char * bufPointer = calloc(BUFSIZE * sizeof(*bufPointer), sizeof(char)); //allocate space for this line
        if(!bufPointer)
        {
            fprintf(stderr, "Error: Memory allocation failed.");
            exit(1);
        }
        else
        {
            int c;
            node * headCommand;
            int charCounter = 0;
            int commandLine = 0;
            int targetLine = 0;
            int validLine = 1;

            lineCounter++;
        
            while ((c = fgetc(makeFile)) != EOF && c != '\n') 
            {
                if(charCounter < BUFSIZE-1)//if we are within the buffer size 
                {
                    if(c == 0)//null char 
                    {
                        validLine = 0;
                    }
                    if(charCounter == 0)
                    {
                        if(c == '\t') //this is a command line 
                        {
                            commandLine = 1;
                            if(hasTarget == 0) //fails if there is a command line with no target line to go with
                            {
                                validLine = 0;
                                commandLine = 0;
                            }
                        }
                        else if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) //this is a target line
                        {
                            targetLine = 1;
                            hasTarget = 1;
                        }
                        else if(c == '#') //this line is a comment or a newline - we'll disregard it but it isn't invalid
                        {
                            validLine = 1;
                        }
                        else if(c == '\n' || c == EOF) //if the first char is a new line or EOF, just quit
                        {
                            break;
                        }
                        else
                        {
                            validLine = 0;
                        }
                    }
                    
                    bufPointer[charCounter] = c;                  
                }
                else //if we exceed the buffer size 
                {
                    fprintf(stderr, "%d: Invalid line - line contents exceed buffer size: %s\n", lineCounter, bufPointer); 
                    return 1;
                }
                charCounter++;
            }  
            
            if(validLine == 0)
            {   
                fprintf(stderr, "%d: Invalid line: \"%s\"\n", lineCounter, bufPointer); 
                free(bufPointer);
                bufPointer = NULL;
                return 1;   
            }
            else
            {
                if(c == EOF)
                {
                    notEOF = 0;    
                    if(currentBuildSpec != NULL && targetLine == 0 && commandLine == 0)
                    {
                        addBuildSpecToGraph(currentBuildSpec); 
                    }               
                }
                else if(c == '\n')
                {
                    bufPointer[charCounter] = '\n';
                    charCounter++;
                }
                
                bufPointer[charCounter] = '\0';//null terminate the string
                
                if(targetLine == 1)
                {
                    //if this is a new target
                    //add the old target to the graph
                    if(currentBuildSpec != NULL)
                    {
                        addBuildSpecToGraph(currentBuildSpec);
                    }                 
                    
                    //create a new one
                    currentBuildSpec = processTargetLine(bufPointer);
                    if(currentBuildSpec != NULL)
                    {
                        if(notEOF == 0 && currentBuildSpec != NULL)
                        { 
                            addBuildSpecToGraph(currentBuildSpec); 
                        }
                    }
                    else
                    {
                        fprintf(stderr, "%d: Invalid line: \"%s\"\n", lineCounter, bufPointer);
                        free(bufPointer);
                        bufPointer = NULL;   
                        return 1; 
                    }
                }
                else if(commandLine == 1)
                {                    
                    if(currentBuildSpec)
                    {
                        //if this is a new command
                        headCommand = processCommandLine(bufPointer, currentBuildSpec);
                        if(headCommand != NULL)
                        {
                            if(notEOF == 0 && currentBuildSpec != NULL)
                            {
                                addBuildSpecToGraph(currentBuildSpec); 
                            }
                        }
                        else
                        {
                            fprintf(stderr, "%d: Invalid line: \"%s\"\n", lineCounter, bufPointer);
                            free(bufPointer);
                            bufPointer = NULL;   
                            return 1; 
                        }
                    }
                    else
                    {
                        fprintf(stderr, "%d: Invalid line: \"%s\"\n", lineCounter, bufPointer);
                        free(bufPointer);
                        bufPointer = NULL;   
                        return 1; 
                    }
                }

                free(bufPointer);
                bufPointer = NULL;
            }
        }
    }
    return 0;
}

//process a target line
//each new target line also gets a new buildSpec object!
graphNode * processTargetLine(char * line)
{
    node * head = NULL;
    int c = 0;
    int colonFound = 0;
    int validLine = 1;
    int wordCounter = 0;
    int wordStart = 0;
    int numSpaces = 0;
    int EOFtag = 0;
    int len = 0;
    
    if(line != NULL)
    {
        len = strlen(line);
    }
    else
    {
        return NULL;
    } 
    
    for (int i = 0; line[i] != 0; i++)
    {
        if(i <= len)
        {
          c = line[i];
          
          //find colon
          if(colonFound == 0)
          {
              if(c == ' ' || c == '\t')
              {
                  numSpaces++; 
              }
              if(c == ':')
              {
                  colonFound = 1;
                  validLine = 1;
                  char * target = malloc((i - wordStart) * sizeof (char*) + 1);
                  if(sprintf(target, "%.*s", i - wordStart - numSpaces, line + wordStart) < 0)
                  {
                      fprintf(stderr, "Error: sprintf failed.");
                      exit(1);
                  }
                  target[(i - wordStart) * sizeof(*target)] = '\0';
                  head = createNode(target); //this word is the target
                  wordStart = i+1;//skip colon
                  wordCounter++;
                  numSpaces = 0; //reset num spaces
              }
              else if( c!= ' ' && c!= '\t' && c!= ':') //if there are multiple targets, consider this an invalid line 
              {
                  if(numSpaces > 0) 
                  {
                      return NULL;
                  }
              }
          }
          //find targets and dependencies
          else if(colonFound == 1) 
          {
              if(c == ':' || c == '\t') //should contain no colons and only spaces as separators
              {
                  return NULL;
              }
              else if(c == ' ' || i == (int)strlen(line)-1 )
              {
                  if(line[i - 1] != ' ' && line[i - 1] != ':' && line[i-1] != '\t')//if the last char wasn't a space
                  {
                      if(i == (int)strlen(line)-1)
                      {
                          if(c != '\n')
                          {
                              EOFtag = 1;                            
                          }
                      }
                      wordCounter++;
                      if(wordCounter == 1)//if this is the first word
                      {
                          
                      }
                      else //if this is not the first word 
                      {
                          char * depend = malloc((i - wordStart) * sizeof (char*)+1);
                          if(sprintf(depend, "%.*s", i - wordStart - numSpaces + EOFtag, line + wordStart + numSpaces) < 0)
                          {
                              fprintf(stderr, "Error: sprintf failed.");
                              exit(1);
                          }
                          depend[(i - wordStart) * sizeof(*depend)] = '\0';
                          addToEnd(&head, depend);
                          wordStart = i;  
                                                  
                      }
                      numSpaces = 1; //reset number of spaces 
                      if(i == (int)strlen(line)-1)
                      {
                          i = i + 1;
                      }                                      
                  }
                  else
                  {
                      //we will ignore this char and keep ignoring spaces until we get to the next start of a dependence
                      numSpaces++; 
                  }
              }
          
          }
        }
    }    
    if(validLine == 1)
    {
        graphNode * newSpec = createBuildSpec(head,NULL);
        return newSpec; 
    }
    else
    {
        return NULL; 
    }
}



node * processCommandLine(char * line, graphNode * currentBuildSpec)
{
    char c = 0;
    int validLine = 1;
    int wordCounter = 0;
    int wordStart = 0;
    int numSpaces = 0;
    int EOFtag = 0;

    for (int i = 0; line[i] != 0; i++)
    {
        c = line[i];        
        
        if(i == 1 && (c == '\t' || c == ' ' || c == '\n' || c =='#')) //if extra tabs or spaces before command 
        {
            return NULL;
        }
        
        if((c == ' ' || c == '\t' || i == (int)strlen(line)-1) && i != 0) 
        {
            if(line[i - 1] != ' ' && line[i - 1] != '\t')//if the last char wasn't a space
            {
                if(i == (int)strlen(line)-1)
                {
                    i = i - 0;
                    if(c != '\n')
                    {
                        EOFtag = 1;                            
                    }
                }
                wordCounter++;
                if(wordCounter == 1)//if this is the first word
                {
                    char * command = malloc((i - wordStart) * sizeof(*command)+1);
                    if(sprintf(command, "%.*s", i - 1 - wordStart, line + wordStart + 1) < 0)//skip tab
                    {
                        fprintf(stderr, "Error: sprintf failed.");
                        exit(1);
                    }
                    command[(i - wordStart) * sizeof(*command)] = '\0';
                    addToEnd(&currentBuildSpec->commands, command); //this word is the target
                    wordStart = i;
                }
                else //if this is not the first word
                {
                    char * arg = malloc((i - wordStart) * sizeof(*arg)+1);
                    if(sprintf(arg, "%.*s", i - wordStart - numSpaces + EOFtag, line + wordStart + numSpaces) < 0)
                    {
                        fprintf(stderr, "Error: sprintf failed.");
                        exit(1);
                    }
                    arg[(i - wordStart) * sizeof(*arg)] = '\0';
                    addToEnd(&currentBuildSpec->commands, arg); 
                    wordStart = i;
                }
                numSpaces = 1; //reset                                   
            }
            else
            {
                //we will ignore this char and keep ignoring spaces until we get to the next start of a dependence
                numSpaces++;
            }
        }
    }
    if(validLine == 0)
    {
        return NULL;
    }
    else
    {
        addToEnd(&currentBuildSpec->commands, NULL); //add a null separator
        return currentBuildSpec->commands;
    }
}
/*
void * flushLine(char * bufPointer) 
{
    for (int i = 0; bufPointer[i] != '\n' && bufPointer[i] != EOF; i++)
    {
        printf("%c", bufPointer[i]);
    }
    
    return 0;
}*/
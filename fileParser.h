#ifndef FILEPARSER_H_
#define FILEPARSER_H_
#include "linkedList.h"
#include "buildSpec.h"
#include "dependencyGraph.h"

int parseFile(FILE * makeFile);
graphNode * processTargetLine(char * line);
node * processCommandLine(char * line, graphNode * currentBuildSpec);
void * flushLine(char * bufPointer);

#endif // FILEPARSER_H_
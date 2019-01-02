
#ifndef COMMANDEXEC_H_
#define COMMANDEXEC_H_

int runTarget(graphNode * target);
graphNode * findProcess(char * processName, graphNode * graphTop); 
int runCommand(node * command);

#endif // COMMANDEXEC_H_